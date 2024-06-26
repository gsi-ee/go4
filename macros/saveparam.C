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


#include <iostream>
using namespace std;

// Recursive iterator to build a TList of all found objects
// In case of CINT full names are in the TList (needed for Get)
// File must be open
// In case of GO4 GUI parameter names are in TList
// In case of analysis the object pointers are in the TList

void paramiter(TDirectory *dir, const char *wildcard, TList *found)
{
#ifdef __GO4MACRO__
   TRegexp wild(wildcard, kTRUE);
   TGo4Iter iter(go4->Browser()->BrowserSlot("Analysis"), kFALSE);
   while(iter.next()) {
      if(!iter.isfolder() && (TString(iter.getname()).Index(wild) != kNPOS)) {
         TString itemname = "Analysis/";
         itemname.Append(iter.getfullname());
         if (go4->Browser()->ItemKind(itemname.Data()) == TGo4Access::kndGo4Param) {
            go4->FetchItem(itemname.Data(), 1000);
            TObject *obj = go4->GetObject(itemname.Data());
            if (obj) found->Add(obj);
         }
      }
   }
#endif
#ifdef __GO4ANAMACRO__
   Bool_t reset = kTRUE;
   TObject *obj = nullptr;
   while((obj = go4->NextMatchingObject(wildcard,"Go4",reset)) != nullptr) {
      reset = kFALSE;
      if (obj->InheritsFrom("TGo4Parameter")) found->Add(obj);
   }
#endif
#ifdef __NOGO4MACRO__
   if (!dir) return;
   // found->SetOwner(kTRUE);
   TRegexp wild(wildcard, kTRUE);
   TIter next(dir->GetListOfKeys());
   while(auto key = (TKey *)next()) {
      if(strcmp(key->GetClassName(),"TDirectoryFile") == 0)
         paramiter(dir->GetDirectory(key->GetName()), wildcard, found);
      else
         if (TString(key->GetName()).Index(wild) != kNPOS) {
            TClass *cl = TClass::GetClass(key->GetClassName());
            if (cl && cl->InheritsFrom("TGo4Parameter")) {
                TObject *obj = dir->Get(key->GetName());
                if (obj) found->Add(obj);
            }
         }
   }
#endif
}

TString MakeParamFuncName(const char *main, const char *objname)
{
   TString subfunc;
   subfunc.Form("%s_%s", main, objname);
   subfunc.ReplaceAll("#","_");
   subfunc.ReplaceAll("(","_");
   subfunc.ReplaceAll(")","_");
   subfunc.ReplaceAll("*","_");
   subfunc.ReplaceAll("@","_");
   subfunc.ReplaceAll("[","_");
   subfunc.ReplaceAll("]","_");
   return subfunc;
}

// Function to process one parameter
// outside Go4 get parameter from file (1st arg)
Bool_t save1param(TObject *obj, const char *prefix)
{
  if (!obj || !obj->InheritsFrom("TGo4Parameter")) return kFALSE;

  TGo4Parameter *param = (TGo4Parameter*) obj;

  TString funcname = MakeParamFuncName(prefix, param->GetName());

  std::cout << "Write macro " << funcname << std::endl;
  std::ofstream xout(funcname+".C");

  xout << "// written by macro saveparam.C at " << TDatime().AsString() << std::endl;
  xout << TString::Format("void %s()", funcname.Data()) << std::endl;
  xout << "{" << std::endl;
  xout << "#ifndef __GO4ANAMACRO__" << std::endl;
  xout << TString::Format("   std::cout << \"Macro %s can execute only in analysis\" << std::endl;", funcname.Data()) << std::endl;
  xout << "   return;" << std::endl;
  xout << "#endif" << std::endl;

  param->SavePrimitive(xout, "savemacro");

  xout << std::endl;
  xout << "}" << std::endl;
  xout.close();
  return kTRUE;
}

#ifdef __NOGO4MACRO__
// Get objects from ROOT file
void saveparam(const char *file, const char *wildcard = "*", const char *prefix = "set")
{
  TFile *f = TFile::Open(file,"r");
#else
void saveparam(const char *wildcard = "*", const char *prefix = "set")
{
  TFile *f = nullptr;
  const char *file = nullptr;
#endif

  TList lst;
  paramiter(f, wildcard, &lst);
  if (f) { delete f; f = nullptr; }

  TIter next(&lst);

  TObject *obj = nullptr;
  while((obj = next()) != nullptr)
     save1param(obj, prefix);

  lst.Clear();
}
