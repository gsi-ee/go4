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
#include "Riostream.h"
using namespace std;

#include "RVersion.h"

// Recursive iterator to build a TList of all found objects
// In case of CINT full names are in the TList (needed for Get)
// File must be open
// In case of GO4 GUI parameter names are in TList
// In case of analysis the object pointers are in the TList

void paramiter(TDirectory *dir, const char* wildcard, TList* found)
{
#ifdef __GO4MACRO__
   TRegexp wild(wildcard, kTRUE);
   TGo4Iter iter(go4->Browser()->BrowserSlot("Analysis"), kFALSE);
   while(iter.next()) {
      if(!iter.isfolder() && (TString(iter.getname()).Index(wild) != kNPOS)) {
         TString itemname = Form("Analysis/%s", iter.getfullname());
         if (go4->Browser()->ItemKind(itemname.Data()) == TGo4Access::kndGo4Param) {
            go4->FetchItem(itemname.Data(), 1000);
            TObject* obj = go4->GetObject(itemname.Data());
            if (obj) found->Add(obj);
         }
      }
   }
#endif
#ifdef __GO4ANAMACRO__
   Bool_t reset = kTRUE;
   TObject* obj = 0;
   while((obj = go4->NextMatchingObject(wildcard,"Go4",reset))!=0) {
      reset = kFALSE;
      if (obj->InheritsFrom("TGo4Parameter")) found->Add(obj);
   }
#endif
#ifdef __NOGO4MACRO__
   if (dir==0) return;
   // found->SetOwner(kTRUE);
   TRegexp wild(wildcard, kTRUE);
   TIter next(dir->GetListOfKeys());
   TKey *key = 0;
   while((key=(TKey*)next())) {
      if(strcmp(key->GetClassName(),"TDirectoryFile")==0)
         paramiter(dir->GetDirectory(key->GetName()), wildcard, found);
      else
         if (TString(key->GetName()).Index(wild) != kNPOS) {
            TClass* cl = TClass::GetClass(key->GetClassName());
            if ((cl!=0) && cl->InheritsFrom("TGo4Parameter")) {
                TObject* obj = dir->Get(key->GetName());
                if (obj) found->Add(obj);
            }
         }
   }
#endif
}

TString MakeFuncName(const char* main, const char* objname)
{
   TString subfunc = Form("%s_%s", main, objname);
   subfunc.ReplaceAll("#","_");
   subfunc.ReplaceAll("(","_");
   subfunc.ReplaceAll(")","_");
   subfunc.ReplaceAll("*","_");
   subfunc.ReplaceAll("@","_");
   return subfunc;
}

// Function to process one parameter
// outside Go4 get parameter from file (1st arg)
Bool_t save1param(TObject* obj, const char* prefix)
{
  if ((obj==0) || !obj->InheritsFrom("TGo4Parameter")) return kFALSE;

  TGo4Parameter* param = (TGo4Parameter*) obj;

  TString funcname = MakeFuncName(prefix, param->GetName());

  cout << "Write macro " << funcname << endl;
  ofstream xout(funcname+".C");

  xout << Form("// written by macro saveparam.C at %s",TDatime().AsString()) << endl;
  xout << Form("void %s()", funcname.Data()) << endl;
  xout << Form("{") << endl;
  xout << Form("#ifndef __GO4ANAMACRO__") << endl;
  xout << Form("   cout << \"Macro %s can execute only in analysis\" << endl;", funcname.Data()) << endl;
  xout << Form("   return;") << endl;
  xout << Form("#endif") << endl;

  param->SavePrimitive(xout, "savemacro");

  xout << endl;
  xout << "}" << endl;
  xout.close();
  return kTRUE;
}

#ifdef __NOGO4MACRO__
// Get objects from ROOT file
void saveparam(const char* file, const char* wildcard = "*", const char* prefix = "set")
{
  TFile *f = TFile::Open(file,"r");
#else
void saveparam(const char* wildcard = "*", const char* prefix = "set")
{
  TFile *f = 0;
  const char* file = 0;
#endif

  TList lst;
  paramiter(f, wildcard, &lst);
  if (f!=0) { delete f; f = 0; }

  TIter next(&lst);

  TObject* obj = 0;
  while((obj = next()) != 0)
     save1param(obj, prefix);

  lst.Clear();
}


