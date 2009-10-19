// Macro saveparam.C searches all parameter objects
// in a file, the analysis folders, or the Go4 GUI Analysis folders
// It writes a macro for each found parameter setting the current values.
// The generated macro has the name pref_parametername.C
// It can execute only in analysis environment
// Running in ROOT (needs .rootmap or Go4 libraries loaded)
//     saveparam.C(rootfile,wildcard,prefix)
// Running in Go4 GUI or analysis
//     saveparam.C(wildcard,prefix)
// Wildcard applies only to parameter name without folders
// H.G.Essel July 2006
// Use TGo4Parameter::MakeScript method to convert parameter values into macro 
// S.Linev October 2009

#ifndef __GO4MACRO__
#ifndef __GO4ANAMACRO__
#define __NOGO4MACRO__
#endif
#endif

#include <fstream.h>
#include <Riostream.h>
#include "RVersion.h"
#define kndGo4Param 4
#define WO xout.write(fLine.Data(),fLine.Length())

// Recoursive iterator to build a TList of all found objects
// In case of CINT full names are in the TList (needed for Get)
// File must be open
// In case of GO4 GUI parameter names are in TList
// In case of analysis the object pointers are in the TList

void namiter(TFile *f, TString fulldir, const char* wildcard, TList* found)
{
   TString fullname;
   TString curname;
   TRegexp wild(wildcard,kTRUE);
#ifdef __GO4MACRO__
   found->SetOwner(kTRUE);
   TGo4BrowserProxy *brow = go4->Browser();
   TGo4Slot *slot = brow->BrowserSlot("Analysis");
   TGo4Iter *iter = new TGo4Iter(slot,kFALSE);
   while(iter->next()) {
      if(!iter->isfolder()) {
         curname.Form(iter->getname());
         if(curname.Index(wild) != kNPOS)
            found->Add((TObject *)new TObjString(curname));
      }
   }
#endif
#ifdef __GO4ANAMACRO__
   Bool_t reset=kTRUE;
   while((obj=go4->NextMatchingObject(wildcard,"Go4",reset))!=0) {
      reset=kFALSE;
      found->Add(obj);
   }
#endif
#ifdef __NOGO4MACRO__
   found->SetOwner(kTRUE);
   TKey *key;
   TString curdir;
   TIter next(gDirectory->GetListOfKeys());
   while((key=(TKey*)next())) {
      if(strcmp(key->GetClassName(),"TDirectory")==0) {
         curdir.Form(fulldir.Data());
         curdir.Append(key->GetName());
         curdir.Append("/");
         f->cd(curdir.Data());
         namiter(f,curdir,wildcard,found);
      } else {
         curname.Form("%s",key->GetName());
         fullname.Form("%s%s",fulldir.Data(),key->GetName());
         if(curname.Index(wild) != kNPOS)
            found->Add((TObject *)new TObjString(fullname));
      }
   }
#endif
}


// Function to process one parameter
// outside Go4 get parameter from file (1st arg)
Bool_t save1param(const char* rootfile, const char* name, const char* pref)
{
  TString fLine;
  TString fData;
  TString sName;
  TString setpar;
#ifdef __NOGO4MACRO__
  TFile *f = TFile::Open(rootfile,"r");
  if(f == 0) {
      cout <<"saveparam could not open file " << rootfile << endl;
      return kFALSE;
  }
  // get parameter from file
  TGo4Parameter* param = f->Get(name);
#endif
#ifdef __GO4MACRO__
  // Get parameter from GO4 GUI
  TGo4BrowserProxy *brow = go4->Browser();
  TString fName = go4->FindItem(name);
  if(brow->ItemKind(fName.Data())!=kndGo4Param) return kFALSE;
  go4->FetchItem(fName.Data(),1000);
  TGo4Parameter *param = go4->GetObject(fName);
#endif
#ifdef __GO4ANAMACRO__
  // get parameter from Go4 analysis
  TGo4Parameter *param = go4->GetObject(name,"Go4");
#endif
  if(param && param->InheritsFrom("TGo4Parameter"))
       sName=param->GetName();
  else return kFALSE;

  setpar.Form("%s_%s.C",pref,sName.Data());
  cout << "Write macro " << setpar.Data() << endl;
  ofstream xout(setpar.Data());
  TClass *parclass = param->IsA();
  TDatime datime;
  fLine.Form("// written by macro saveparam.C at %s\n",datime.AsString());  WO ;
  fLine.Form("#include \"Riostream.h\"\n");  WO ;
  fLine.Form("Bool_t %s_%s()\n",pref,sName.Data());  WO ;
  fLine.Form("{\n");  WO ;
  fLine.Form("#ifndef __GO4ANAMACRO__\n");  WO ;
  fLine.Form("   cout << \"Macro %s_%s can execute only in analysis\" << endl;\n",
        pref,sName.Data());  WO ;
  fLine.Form("   return kFALSE;\n");  WO ;
  fLine.Form("#endif\n");  WO ;
  fLine.Form("   %s* param = (%s*) go4->GetObject(\"%s\",\"Go4\");\n\n",
             parclass->GetName(),parclass->GetName(),sName.Data());  WO ;
  fLine.Form("   if(param==0) {\n");  WO ;
  fLine.Form("      cout << \"%s: could not find %s\" << endl;\n",setpar.Data(),sName.Data());  WO ;
  fLine.Form("      return kFALSE;\n");  WO ;
  fLine.Form("   }\n\n");  WO ;
  fLine.Form("   if(strcmp(param->ClassName(),\"%s\")!=0) {\n",param->ClassName());  WO ;
  fLine.Form("      cout << \"%s: %s has wrong class \" << param->ClassName() << endl;\n",
        setpar.Data(),sName.Data());  WO ;
  fLine.Form("      return kFALSE;\n");  WO ;
  fLine.Form("   }\n\n");  WO ;
  fLine.Form("   cout << \"Set parameter %s as saved by saveparam.C at %s\" << endl;\n\n",
                    sName.Data(),datime.AsString());  WO ;

  param->MakeScript(xout, "param->", 3);

  fLine.Form("\n");  WO ;
  fLine.Form("   return kTRUE;\n");  WO ;
  fLine.Form("}\n");  WO ;
  xout.close();
#ifdef __NOGO4MACRO__
  f->Close();
#endif
  return kTRUE;
}

#ifdef __NOGO4MACRO__
// Get objects from ROOT file
void saveparam(const char* file, const char* wildcard = "*", const char* pref = "save")
{
  TFile *f = TFile::Open(file,"r");
#else
void saveparam(const char* wildcard = "*", const char* pref = "save")
{
  TFile *f = 0;
  const char* file = 0;
#endif
  TList lst;
  TString fulldir;
  namiter(f, fulldir, wildcard, &lst);
  if (f!=0) { delete f; f = 0; }

  TIter next(&lst);

  TObject* namo = 0;
  while((namo = next()) != 0)
     save1param(file, namo->GetName(), pref);

  lst.Clear();
}


