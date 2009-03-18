#include "Riostream.h"

///////////////////////////////////////////////////////////////////
//////// Go4 GUI example script rebin.C
//          J.Adamczewski, gsi, May 2006
// NOTE: to be run in Go4 GUI local command line only!
//       NEVER call this script in remote analysis process!!!
/////// Functionality:
// rebins histogram of name1 by merging ngroup neighboured channels
/////// Usage:
// The draw flag switches if the results are displayed each time this makro is called
// if display is switched off, the result histogram is just updated in browser and existing displays
///////
Bool_t rebin(const char* name1, Int_t ngroup = 2, Bool_t draw)
{
if(TGo4AbstractInterface::Instance()==0 || go4!=TGo4AbstractInterface::Instance())
   {
      cout <<"FATAL: Go4 gui macro executed outside Go4 GUI!! returning." << endl;
      return;
   }
TString fullname1 = go4->FindItem(name1);
TObject* ob1=go4->GetObject(fullname1,1000); // 1000=timeout to get object from analysis in ms
TH1* his1=0;
if(ob1 && ob1->InheritsFrom("TH1"))
   his1 = (TH1*)ob1;
if(his1==0)
   {
      cout <<"rebin could not get histogram "<<fullname1 << endl;
      return kFALSE;
   }

TString n1=his1->GetName();
TString t1=his1->GetTitle();
TString operator;
operator.Form("_rebinned_%d",ngroup);
TString finalname=n1+operator;
TString finaltitle=t1+operator;
TH1* result= his1->Rebin(ngroup,finalname);
result->SetTitle(finaltitle);
result->SetDirectory(0);
rname = go4->SaveToMemory("Rebins", result, kTRUE);
cout<< "Saved result histogram to " << rname.Data() <<endl;
if(!draw) return kTRUE;
ViewPanelHandle vpanel = go4->StartViewPanel();
go4->SetSuperimpose(vpanel,kTRUE);
go4->DrawItem(fullname1, vpanel);
go4->DrawItem(rname, vpanel);
//go4->DrawItem(rname);
return kTRUE;
}