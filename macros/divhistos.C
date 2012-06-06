#include "Riostream.h"
using namespace std;


///////////////////////////////////////////////////////////////////
//////// Go4 GUI example script addhistos.C
//       J.Adamczewski, gsi, May 2006
// NOTE: to be run in Go4 GUI local command line only!
//       NEVER call this script in remote analysis process!!!
/////// Functionality:
// divide histogram of name 1 by histogram of name2
/////// Usage:
// The draw flag switches if the results are displayed each time this makro is called
// if display is switched off, the result histogram is just updated in browser and existing displays
///////
Bool_t divhistos(const char* name1, const char* name2, Bool_t draw)
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
      cout <<"divhistos could not get histogram "<<fullname1 << endl;
      return kFALSE;
   }
TString fullname2 = go4->FindItem(name2);
TObject* ob2=go4->GetObject(fullname2,1000); // 1000=timeout to get object from analysis in ms
TH1* his2=0;
if(ob2 && ob2->InheritsFrom("TH1"))
   his2 = (TH1*)ob2;
if(his2==0)
   {
      cout <<"divhistos could not get histogram "<<fullname2 << endl;
      return kFALSE;
   }
TH1* result= (TH1*) his1->Clone();

TString n1=his1->GetName();
TString n2=his2->GetName();
TString t1=his1->GetTitle();
TString t2=his2->GetTitle();
TString operator;
operator.Form(") : (");
TString finalname="("+n1+operator+n2+")";
TString finaltitle="("+t1+operator+t2+")";
result->SetName(finalname);
result->SetTitle(finalname);
result->Sumw2();
result->Divide(his2);
result->SetDirectory(0);
rname = go4->SaveToMemory("Divs", result, kTRUE);
cout<< "Saved result histogram to " << rname.Data() <<endl;
if(!draw) return kTRUE;
go4->DrawItem(rname);
return kTRUE;
}
