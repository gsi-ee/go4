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
  TGo4Condition* con;
  TGo4WinCond*   win;
  TGo4PolyCond*  pol;
  TGo4CondArray* ar;
  TObject*       obj=0;
  TCutG*         cut;
  Double_t       xl,xu,yl,yu,x[200],y[200];
  Double_t*      xs,*ys;
  Bool_t         enabled,last,mark,result,vtrue,vfalse,reset=kTRUE;
  Int_t          dim,counts,tcounts,maxi,ii,ip,points;
  TString fLine;
  TString fValue;
  TString fData;
  TString fName;
  TString sName;
  TString setcon;

#ifdef __NOGO4MACRO__
  TFile *f = TFile::Open(rootfile,"r");
  if(f == 0)
    {
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
  con=cond;
  if(con && con->InheritsFrom("TGo4Condition"))
    sName=con->GetName();
  else return kFALSE;

  setcon.Form("%s_%s.C",pref,sName.Data());
  cout << "Write macro " << setcon.Data() << endl;
  ofstream xout(setcon.Data());
  TClass *conclass = con->IsA();
  TDatime datime;
  fLine.Form("// written by macro savecond.C at %s\n",datime.AsString());  WO ;
  fLine.Form("#include \"Riostream.h\"\n");  WO ;
  fLine.Form("Bool_t %s_%s(Bool_t flags = kTRUE, Bool_t counters = kTRUE, Bool_t reset = kTRUE)\n",pref,sName.Data());  WO ;
  fLine.Form("{\n");  WO ;
  fLine.Form("   TGo4Analysis*  anl;\n");  WO ;
  fLine.Form("   TGo4WinCond*   win;\n");  WO ;
  fLine.Form("   TGo4PolyCond*  pol;\n");  WO ;
  fLine.Form("   TGo4CondArray* ar;\n");  WO ;
  fLine.Form("   Double_t  x[200],y[200];\n");  WO ;
  fLine.Form("#ifndef __GO4ANAMACRO__\n");  WO ;
  fLine.Form("   cout << \"Macro %s_%s can execute only in analysis\" << endl;\n", pref,sName.Data());  WO ;
  fLine.Form("   return kFALSE;\n");  WO ;
  fLine.Form("#endif\n");  WO ;
  fLine.Form("   TGo4Condition* con = (TGo4Condition*) go4->GetObject(\"%s\",\"Go4\");\n",sName.Data());  WO ;
  fLine.Form("   if(con==0) {\n");  WO ;
  fLine.Form("      cout << \"%s: could not find %s\" << endl;\n",setcon.Data(),sName.Data());  WO ;
  fLine.Form("      return kFALSE;\n");  WO ;
  fLine.Form("   }\n\n");  WO ;
  fLine.Form("   if(strcmp(con->ClassName(),\"%s\")) {\n",con->ClassName());  WO ;
  fLine.Form("   cout << \"%s: %s has wrong class \" << con->ClassName() << endl;\n",
             setcon.Data(),sName.Data());  WO ;
  fLine.Form("   return kFALSE;\n");  WO ;
  fLine.Form("   }\n");  WO ;
  fLine.Form("   cout << \"Set condition %s as saved by savecond.C at %s\" << endl;\n",
             sName.Data(),datime.AsString()); WO ;
  fLine.Form("// SetFlags(enabled,last,mark,result,vtrue,vfalse);\n"); WO ;

// Array -----------------------------------------------
  if(strcmp(con->ClassName(),"TGo4CondArray") == 0)
  {
   fLine.Form("\nar=(TGo4CondArray *) con;\n"); WO ;
   ar=(TGo4CondArray *)con;

   if(ar->IsPolygonType())for(ii=0;ii<ar->GetNumber();ii++)
     {
       pol=(TGo4PolyCond *)ar->At(ii);
       counts=pol->Counts();
       tcounts=pol->TrueCounts();
       pol->GetFlags(&enabled, &last, &mark, &result, &vtrue, &vfalse);
       cut=pol->GetCut(kFALSE); // no owner
       xs=cut->GetX();ys=cut->GetY();points=cut->GetN();
       fLine.Form("\npol=(TGo4PolyCond *)ar->At(%d);\n",ii); WO ;
       fLine.Form("if(flags)   pol->SetFlags(%d,%d,%d,%d,%d,%d);\n",
                 enabled,last,mark,result,vtrue,vfalse); WO ;
       fLine.Form("if(counters)pol->SetCounts(%d,%d);\n",tcounts,counts); WO ;
       fLine.Form("if(reset)   pol->ResetCounts();\n"); WO ;
       for(ip=0;ip<points;ip++)
    {
      fLine.Form("   x[%d] = %f; y[%d] = %f;\n",ip,*xs,ip,*ys); WO ;
      x[ip] = *xs; y[ip] = *ys;
      xs++; ys++;
    }
       fLine.Form("pol->SetValues(x,y,%d);\n",points); WO ;
     } // end polygon

   else for(ii=0;ii<ar->GetNumber();ii++)
     {
       win=(TGo4WinCond *)ar->At(ii);
       win->GetValues(dim,xl,xu,yl,yu);
       counts=win->Counts();
       tcounts=win->TrueCounts();
       win->GetFlags(&enabled, &last, &mark, &result, &vtrue, &vfalse);
       win->SetFlags(enabled,last,mark,result,vtrue,vfalse);
       win->SetCounts(tcounts,counts);
       fLine.Form("\nwin=(TGo4WinCond *)ar->At(%d);\n",ii); WO ;
       fLine.Form("if(flags)   win->SetFlags(%d,%d,%d,%d,%d,%d);\n",
                 enabled,last,mark,result,vtrue,vfalse); WO ;
       fLine.Form("if(counters)win->SetCounts(%d,%d);\n",tcounts,counts); WO ;
       fLine.Form("if(reset)   win->ResetCounts();\n"); WO ;
       if(dim==1){fLine.Form("win->SetValues(%f,%f);\n",xl,xu); WO ;}
       if(dim==2){fLine.Form("win->SetValues(%f,%f,%f,%f);\n",xl,xu,yl,yu); WO ;}
     } // end window
 } // end array

 if(strcmp(con->ClassName(),"TGo4WinCond")   == 0)
 {
   win=(TGo4WinCond *)con;
   win->GetValues(dim,xl,xu,yl,yu);
   counts=win->Counts();
   tcounts=win->TrueCounts();
   win->GetFlags(&enabled, &last, &mark, &result, &vtrue, &vfalse);
   fLine.Form("\nwin=(TGo4WinCond *) con;\n"); WO ;
   fLine.Form("if(flags)   win->SetFlags(%d,%d,%d,%d,%d,%d);\n",
             enabled,last,mark,result,vtrue,vfalse); WO ;
   fLine.Form("if(counters)win->SetCounts(%d,%d);\n",tcounts,counts); WO ;
   fLine.Form("if(reset)   win->ResetCounts();\n"); WO ;
   if(dim==1){fLine.Form("win->SetValues(%f,%f);\n",xl,xu); WO ;}
   if(dim==2){fLine.Form("win->SetValues(%f,%f,%f,%f);\n",xl,xu,yl,yu); WO ;}
 } // end window

 if(strcmp(con->ClassName(),"TGo4PolyCond")  == 0)
{
   pol=(TGo4PolyCond *)con;
   counts=pol->Counts();
   tcounts=pol->TrueCounts();
   pol->GetFlags(&enabled, &last, &mark, &result, &vtrue, &vfalse);
   cut=pol->GetCut(kFALSE); // no owner
   xs=cut->GetX();ys=cut->GetY();points=cut->GetN();
   fLine.Form("\npol=(TGo4PolyCond *) con;\n"); WO ;
   fLine.Form("if(flags)   pol->SetFlags(%d,%d,%d,%d,%d,%d);\n",
               enabled,last,mark,result,vtrue,vfalse); WO ;
   fLine.Form("if(counters)pol->SetCounts(%d,%d);\n",tcounts,counts); WO ;
   fLine.Form("if(reset)   pol->ResetCounts();\n"); WO ;
   for(ip=0;ip<points;ip++)
   {
   fLine.Form("   x[%d] = %f; y[%d] = %f;\n",ip,*xs,ip,*ys); WO ;
   x[ip] = *xs; y[ip] = *ys;
   xs++; ys++;
   }
   fLine.Form("pol->SetValues(x,y,%d);\n",points); WO ;
 } // end polygon

   fLine.Form("return kTRUE;\n}\n");  WO ;
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
