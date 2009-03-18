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

void namiter(TFile *f, TString fulldir, Text_t *wildcard, TList* found)
{
  TString fullname;
  TString curname;
  TRegexp wild(wildcard,kTRUE);
#ifdef __GO4MACRO__
    TGo4BrowserProxy *brow = go4->Browser();
    TGo4Slot *slot = brow->BrowserSlot("Analysis");
    TGo4Iter *iter = new TGo4Iter(slot,kFALSE);
    while(iter->next())
      {
   if(!iter->isfolder())
     {
       curname.Form(iter->getname());
       if(curname.Index(wild) != kNPOS)
         found->Add((TObject *)new TObjString(curname));
     }
      }
#endif
#ifdef __GO4ANAMACRO__
    Bool_t reset=kTRUE;
    while((obj=go4->NextMatchingObject(wildcard,"Go4",reset))!=0)
      {
         reset=kFALSE;
    found->Add(obj);
      }
#endif
#ifdef __NOGO4MACRO__
  TKey *key;
  TString curdir;
  TIter next(gDirectory->GetListOfKeys());
  while((key=(TKey*)next()))
    {
      if(strcmp(key->GetClassName(),"TDirectory")==0)
   {
     curdir.Form(fulldir.Data());
     curdir.Append(key->GetName());
     curdir.Append("/");
     f->cd(curdir.Data());
     namiter(f,curdir,wildcard,found);
    }
      else
   {
     curname.Form("%s",key->GetName());
     fullname.Form("%s%s",fulldir.Data(),key->GetName());
     if(curname.Index(wild) != kNPOS)
         found->Add((TObject *)new TObjString(fullname));
   }
    }
#endif
  return;
}


// Function to process one parameter
// outside Go4 get parameter from file (1st arg)
Bool_t save1param(Text_t *rootfile, Text_t *name, Text_t *pref)
{
  unsigned long int cloffset=0;
  TString fLine;
  TString fValue;
  TString fData;
  TString fName;
  TString sName;
  TString setpar;
#ifdef __NOGO4MACRO__
  TFile *f=TFile::Open(rootfile,"r");
  if(f == 0)
    {
      cout <<"saveparam could not open file " << rootfile << endl;
      return kFALSE;
    }
  // get parameter from file
  TGo4Parameter* param=f->Get(name);
#endif
#ifdef __GO4MACRO__
  // Get parameter from GO4 GUI
  TGo4BrowserProxy *brow = go4->Browser();
  fName = go4->FindItem(name);
  if(brow->ItemKind(fName.Data())!=kndGo4Param) return kFALSE;
  go4->FetchItem(fName.Data(),1000);
  TGo4Parameter *param = go4->GetObject(fName);
#endif
#ifdef __GO4ANAMACRO__
  // get parameter from Go4 analysis
  TGo4Parameter *param=go4->GetObject(name,"Go4");
#endif
  if(param && param->InheritsFrom("TGo4Parameter"))
       sName=param->GetName();
  else return kFALSE;

  setpar.Form("%s_%s.C",pref,sName.Data());
  cout << "Write macro " << setpar.Data() << endl;
  ofstream xout(setpar.Data());
  TClass *parclass = param->IsA();
  datime=new TDatime();
  fLine.Form("// written by macro saveparam.C at %s\n",datime->AsString());  WO ;
  fLine.Form("#include \"Riostream.h\"\n");  WO ;
  fLine.Form("Bool_t %s_%s()\n",pref,sName.Data());  WO ;
  fLine.Form("{\n");  WO ;
  fLine.Form("#ifndef __GO4ANAMACRO__\n");  WO ;
  fLine.Form("cout << \"Macro %s%s can execute only in analysis\" << endl;\n",
        pref,sName.Data());  WO ;
  fLine.Form("return kFALSE;\n");  WO ;
  fLine.Form("#endif\n");  WO ;
  fLine.Form("%s *param=(%s *)go4->GetObject(\"%s\",\"Go4\");\n",
             parclass->GetName(),parclass->GetName(),sName.Data());  WO ;
  fLine.Form("if(!param){\n");  WO ;
  fLine.Form("cout << \"%s: could not find %s\" << endl;\n",setpar.Data(),sName.Data());  WO ;
  fLine.Form("return kFALSE;}\n");  WO ;
  fLine.Form("if(strcmp(param->ClassName(),\"%s\")){\n",param->ClassName());  WO ;
  fLine.Form("cout << \"%s: %s has wrong class \" << param->ClassName() << endl;\n",
        setpar.Data(),sName.Data());  WO ;
  fLine.Form("return kFALSE;}\n");  WO ;
  fLine.Form("cout << \"Set parameter %s as saved by saveparam.C at %s\" << endl;\n",
            sName.Data(),datime->AsString());  WO ;

  TObjArray *fitems = new TObjArray();
  fitems->SetOwner(kTRUE);
  param->GetMemberValues(fitems);
  TIterator *fitemI = fitems->MakeIterator();
  TIter iter(parclass->GetListOfDataMembers());
  TObject* obj = 0;

  while ((obj  = iter()) != 0) {
    TDataMember* member = dynamic_cast<TDataMember*>(obj);
    if (member==0) continue;
    const char* memtypename = member->GetFullTypeName();
    Int_t memtypeid = 0;

    // do not edit IsA info
    if(strcmp(memtypename,"TClass*")==0) continue;
    // skip for a moment all types which are not basic types

    if (strcmp(memtypename,"TString")==0)
      memtypeid = TGo4ParameterMember::kTString_t;
    else
      if (strcmp(memtypename,"TGo4Fitter*")==0)
        memtypeid = TGo4ParameterMember::kTGo4Fitter_t;
      else {
        if (!member->IsBasic()) continue;
        memtypeid = member->GetDataType()->GetType();
      }
    Int_t unitsize = member->GetUnitSize();
    Int_t arraydim = member->GetArrayDim();
    Int_t maxindex1 = 1;
    Int_t maxindex2 = 1;
    switch(arraydim) {
    case 1:
      maxindex1 = member->GetMaxIndex(0);
      break;
    case 2:
      maxindex1 = member->GetMaxIndex(0);
      maxindex2 = member->GetMaxIndex(1);
      break;
    } // switch()
    for(Int_t ix1=0;ix1<maxindex1;ix1++)
      for(Int_t ix2=0;ix2<maxindex2;ix2++) {
   TGo4ParameterMember* info =  dynamic_cast<TGo4ParameterMember*> (fitemI->Next());
   char* addr = (char *)param + cloffset + member->GetOffset() + (ix1*maxindex2 + ix2) * unitsize;

   fValue = "";
   if(!strcmp("UInt_t",info->GetTypeName()))     fValue.Form("%u", *((UInt_t*)addr));
   else if(!strcmp("Int_t",info->GetTypeName()))      fValue.Form("%d", *((Int_t*)addr));
   else if(!strcmp("ULong_t",info->GetTypeName()))    fValue.Form("%lu", *((ULong_t*)addr));
   else if(!strcmp("Long_t",info->GetTypeName()))     fValue.Form("%ld", *((Long_t*)addr));
   else if(!strcmp("ULong64_t",info->GetTypeName()))  fValue.Form("%llu", *((ULong64_t*)addr));
   else if(!strcmp("Long64_t",info->GetTypeName()))   fValue.Form("%lld", *((Long64_t*)addr));
   else if(!strcmp("UShort_t",info->GetTypeName()))   fValue.Form("%hu", *((UShort_t*)addr));
   else if(!strcmp("Short_t",info->GetTypeName()))    fValue.Form("%hd", *((Short_t*)addr));
   else if(!strcmp("UChar_t",info->GetTypeName()))    fValue.Form("%u", *((UChar_t*)addr));
   else if(!strcmp("Char_t",info->GetTypeName()))     fValue.Form("%d", *((Char_t*)addr));
#if ROOT_VERSION_CODE >= ROOT_VERSION(4,3,2)
   else if(!strcmp("Bool_t",info->GetTypeName()))     fValue.Form("%d", *((Bool_t*)addr));
#endif
   else if(!strcmp("Float_t",info->GetTypeName()))    fValue.Form("%f", *((Float_t*)addr));
   else if(!strcmp("Double_t",info->GetTypeName()))   fValue.Form("%f", *((Double_t*)addr));
   else if(!strcmp("Double32_t",info->GetTypeName())) fValue.Form("%f", *((Double32_t*)addr));
   else if(!strcmp("TString",info->GetTypeName()))    fValue = *((TString*)addr);

   if(fValue.Length())
     {
       if(maxindex1>1)
         {
      if(maxindex2>1) fLine.Form("param->%s[%d][%d]=",info->GetName(),ix1,ix2);
      else            fLine.Form("param->%s[%d]=",info->GetName(),ix1);
         }
       else                fLine.Form("param->%s=",info->GetName());
       if(!strcmp("TString",info->GetTypeName()))
         fData.Form("\"%s\"; // %s\n",fValue.Data(),info->GetTitle());
       else
         fData.Form("%s; // %s\n",fValue.Data(),info->GetTitle());
       fLine.Append(fData.Data()); WO ;
     }
      } // member loop
  } // while

  fLine.Form("return kTRUE;\n}\n");  WO ;
  xout.close();
#ifdef __NOGO4MACRO__
  f->Close();
#endif
  return kTRUE;
}

#ifdef __NOGO4MACRO__
// Get objects from ROOT file
void saveparam(Text_t *file, Text_t *wildcard, Text_t *pref)
{
  TString fulldir;
  TObject *namo;
  TFile *f = TFile::Open(file,"r");
#else
void saveparam(Text_t *wildcard, Text_t *pref)
{
  TString fulldir;
  TObject *namo;
  TFile *f;
  Text_t *file;
#endif
  TList* list = new TList;
  fulldir.Form("");
  namiter(f,fulldir,wildcard,list);
  TListIter *iter = list->MakeIterator();
#ifdef __NOGO4MACRO__
  f->Close();
#endif
  while(namo = iter->Next())
    {
      save1param(file,namo->GetName(),pref);
    }
}


