// Macro savecond.C searches all condition objects
// in a file, the analysis folders, or the Go4 GUI Analysis folders
// It writes a macro for each found condition setting the current values.
// The generated macro has the name pref_conditionname.C
// It can execute only in analysis environment
// Running in ROOT (needs .rootmap or Go4 libraries loaded)
//     savecond.C(rootfile,wildcard,prefix)
// Running in Go4 GUI or analysis
//     savecond.C(wildcard,prefix)
// Wildcard applies only to condition name without folders
// The generated file can be edited, if needed.
// The generated function has three boolean arguments:
// flags, counters, reset
// to control if flags and counters have to be restored.
// H.G.Essel July 2006

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
// In case of GO4 GUI condition names are in TList
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

// Function to process one condition
// outside Go4 get condition from file (1st arg)
Bool_t save1cond(const char* rootfile, const char* name, const char* pref)
{
  TString fLine, fName, sName, setcon;

#ifdef __NOGO4MACRO__
  TFile *f = TFile::Open(rootfile,"r");
  if(f == 0) {
     cout <<"saveparam could not open file " << rootfile << endl;
     return kFALSE;
   }
  // get condition from file
  TGo4Condition* cond=f->Get(name);
#endif
#ifdef __GO4MACRO__
  // Get condition from GO4 GUI
  TGo4BrowserProxy *brow = go4->Browser();
  fName = go4->FindItem(name);
  TClass *cc = brow->ItemClass(fName.Data());
  if(!cc) return kFALSE;
  if(!cc->InheritsFrom("TGo4Condition")) return kFALSE;
  go4->FetchItem(fName.Data(),1000);
  TGo4Condition* cond = go4->GetObject(fName);
#endif
#ifdef __GO4ANAMACRO__
  // get condition from Go4 analysis
  TGo4Condition* cond=go4->GetObject(name,"Go4");
#endif
  if(cond && cond->InheritsFrom("TGo4Condition"))
    sName=cond->GetName();
  else return kFALSE;

  setcon.Form("%s_%s.C",pref,sName.Data());
  cout << "Write macro " << setcon.Data() << endl;
  ofstream xout(setcon.Data());
  TClass *conclass = cond->IsA();
  TDatime datime;
  fLine.Form("// written by macro savecond.C at %s\n",datime.AsString());  WO ;
  fLine.Form("#include \"Riostream.h\"\n");  WO ;
  fLine.Form("Bool_t %s_%s(Bool_t flags = kTRUE, Bool_t counters = kTRUE, Bool_t reset = kTRUE)\n",pref,sName.Data());  WO ;
  fLine.Form("{\n");  WO ;
  fLine.Form("#ifndef __GO4ANAMACRO__\n");  WO ;
  fLine.Form("   cout << \"Macro %s_%s can execute only in analysis\" << endl;\n", pref,sName.Data());  WO ;
  fLine.Form("   return kFALSE;\n");  WO ;
  fLine.Form("#endif\n\n");  WO ;
  fLine.Form("   %s* con = (%s*) go4->GetObject(\"%s\",\"Go4\");\n",
                  cond->ClassName(), cond->ClassName(), sName.Data());  WO ;
  fLine.Form("   if(con==0) {\n");  WO ;
  fLine.Form("      cout << \"%s: could not find %s\" << endl;\n",setcon.Data(),sName.Data());  WO ;
  fLine.Form("      return kFALSE;\n");  WO ;
  fLine.Form("   }\n\n");  WO ;
  fLine.Form("   if(strcmp(con->ClassName(),\"%s\")) {\n", cond->ClassName());  WO ;
  fLine.Form("      cout << \"%s: %s has wrong class \" << con->ClassName() << endl;\n",
             setcon.Data(),sName.Data());  WO ;
  fLine.Form("      return kFALSE;\n");  WO ;
  fLine.Form("   }\n\n");  WO ;
  fLine.Form("   cout << \"Set condition %s as saved by savecond.C at %s\" << endl;\n",
             sName.Data(),datime.AsString()); WO ;
  fLine.Form("   // SetFlags(enabled,last,mark,result,vtrue,vfalse);\n\n"); WO ;

  cond->MakeScript(xout, "con->", 3, kTRUE);

  fLine.Form("\n   return kTRUE;\n");  WO ;
  fLine.Form("}\n");  WO ;
  xout.close();

#ifdef __NOGO4MACRO__
   f->Close();
#endif
   return kTRUE;
}

#ifdef __NOGO4MACRO__
// Get objects from ROOT file
void savecond(const char* file, const char* wildcard="*", const char* pref="save")
{
   TFile *f = TFile::Open(file,"r");
#else
void savecond(const char* wildcard="*", const char* pref="save")
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
      save1cond(file, namo->GetName(), pref);

   lst.Clear();
}
