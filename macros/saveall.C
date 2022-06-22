// Macro saveall.C searches all parameter and condition objects
// in a file, or in the analysis folders, or the Go4 GUI Analysis folders
// It writes single macro for all objects, setting the current values.
// The generated macro has the name outputname.C
// Generated macro can be execute only in analysis environment
// Macro saveall.C can be run from ROOT for the file:
//     .x saveall.C(rootfile,wildcard,output)
// Or in the Go4 GUI (analysis or GUI context)
//     .x saveall.C(wildcard,output)
// Wildcard applies only to object names
// H.G.Essel July 2006
// S.Linev October 2009

#ifndef __GO4MACRO__
#ifndef __GO4ANAMACRO__
#define __NOGO4MACRO__
#endif
#endif

#include <iostream>
using namespace std;

// Recursive iterator to build a TList of all found objects
// In case of CINT objects taken from the file (file must be open)
// In case of GO4 GUI objects fetched from analysis
// In case of analysis the object pointers taken from TGo4Analysis instance

void namiter(TDirectory *dir, const char* wildcard, TList* found, int classmask = 11)
{
#ifdef __GO4MACRO__
   TRegexp wild(wildcard, kTRUE);
   TGo4Iter iter(go4->Browser()->BrowserSlot("Analysis"), kFALSE);
   while(iter.next()) {
      if(!iter.isfolder() && (TString(iter.getname()).Index(wild) != kNPOS)) {
         TString itemname = "Analysis/";
         itemname.Append(iter.getfullname());
         bool dofetch = false;

         if ((classmask / 10) && (go4->Browser()->ItemKind(itemname.Data()) == TGo4Access::kndGo4Param)) dofetch = true; else
         if (classmask % 10) {
            TClass *cc = go4->Browser()->ItemClass(itemname.Data());
            if(cc && cc->InheritsFrom("TGo4Condition")) dofetch = true;
         }
         if (dofetch) {
            go4->FetchItem(itemname.Data(), 1000);
            TObject* obj = go4->GetObject(itemname.Data());
            if (obj) found->Add(obj);
         }
      }
   }
#endif
#ifdef __GO4ANAMACRO__
   Bool_t reset = kTRUE;
   TObject* obj = nullptr;
   while((obj = go4->NextMatchingObject(wildcard,"Go4",reset)) != nullptr) {
      reset = kFALSE;
      if (((classmask / 10) && obj->InheritsFrom("TGo4Parameter")) ||
          ((classmask % 10) && obj->InheritsFrom("TGo4Condition")))
            found->Add(obj);
   }
#endif
#ifdef __NOGO4MACRO__
   if (!dir) return;
   found->SetOwner(kTRUE);
   TRegexp wild(wildcard,kTRUE);
   TIter next(dir->GetListOfKeys());
   TKey *key = nullptr;
   while((key = (TKey*)next()) != nullptr) {
      if(strcmp(key->GetClassName(),"TDirectoryFile") == 0)
         namiter(dir->GetDirectory(key->GetName()), wildcard, found, classmask);
      else
         if (TString(key->GetName()).Index(wild) != kNPOS) {
            TClass* cl = TClass::GetClass(key->GetClassName());
            if (cl)
               if (((classmask / 10) && cl->InheritsFrom("TGo4Parameter")) ||
                   ((classmask % 10) && cl->InheritsFrom("TGo4Condition"))) {
                  TObject* obj = dir->Get(key->GetName());
                  if (obj) found->Add(obj);
               }
         }
   }
#endif
}

TString MakeFuncName(const char* main, const char* objname)
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

#ifdef __NOGO4MACRO__
// Get objects from ROOT file
void saveall(const char* file, const char* wildcard = "*", const char* outputname = "savemacro", int classmask = 11)
{
  TFile *f = TFile::Open(file,"r");
#else
void saveall(const char* wildcard = "*", const char* outputname = "savemacro", int classmask = 11)
{
  TFile *f = nullptr;
  const char* file = nullptr;
#endif

  TList lst;
  namiter(f, wildcard, &lst, classmask);

  TString macroname;
  macroname.Form("%s.C", outputname);
  std::cout << "Write macro " << macroname.Data() << std::endl;
  std::ofstream xout(macroname.Data());

  xout << TString::Format("// written by macro saveall.C at %s", TDatime().AsString()) << std::endl << std::endl;
  xout << TString::Format("#include \"Riostream.h\"") << std::endl << std::endl;

  TIter next(&lst);

  TString body;
  TObject* obj = nullptr;
  while((obj = next()) != nullptr) {
     if (obj->InheritsFrom("TGo4Parameter")) {
        TString subname = MakeFuncName(outputname, obj->GetName());
        xout << TString::Format("Bool_t %s()", subname.Data()) << std::endl;
        xout << "{" << std::endl;

        obj->SavePrimitive(xout, "savemacro");

        xout << "   return kTRUE;" << std::endl;

        xout << "}" << std::endl << std::endl;

        body.Append(TString::Format("   %s();\n", subname.Data()).Data());
     } else
     if (obj->InheritsFrom("TGo4Condition")) {
        TString subname = MakeFuncName(outputname, obj->GetName());
        xout << TString::Format("Bool_t %s(Bool_t flags = kTRUE, Bool_t counters = kFALSE, Bool_t reset = kFALSE)", subname.Data()) << std::endl;
        xout << "{" << std::endl;

        obj->SavePrimitive(xout, "savemacro");

        xout << "   return kTRUE;" << std::endl;

        xout << "}" << std::endl << std::endl;

        body.Append(TString::Format("   %s();\n", subname.Data()).Data());
     }
  }

  lst.Clear();

  if (f) { delete f; f = nullptr; }

  xout << TString::Format("Bool_t %s()", outputname) << std::endl;
  xout << "{" << std::endl;
  xout << "#ifndef __GO4ANAMACRO__" << std::endl;
  xout << TString::Format("   std::cout << \"Macro %s can be executed only in analysis\" << std::endl;", macroname.Data()) << std::endl;
  xout << "   return kFALSE;" << std::endl;
  xout << "#endif" << std::endl << std::endl;

  xout << body;

  xout << std::endl;
  xout << "   return kTRUE;" << std::endl;
  xout << "}" << std::endl;
  xout.close();
}
