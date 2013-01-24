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
Bool_t rebin(const char* name1, Int_t ngroup1 = 2, Int_t ngroup2 = 0, Bool_t draw = kTRUE)
{
   if(TGo4AbstractInterface::Instance()==0 || go4!=TGo4AbstractInterface::Instance()) {
      std::cout <<"FATAL: Go4 gui macro executed outside Go4 GUI!! returning." << std::endl;
      return kFALSE;
   }
   TString newname;
   if(ngroup1<1) ngroup1=1;
   if(ngroup2<1) ngroup2=1;
   TString fullname1 = go4->FindItem(name1);
   TObject* ob1 = go4->GetObject(fullname1,1000); // 1000=timeout to get object from analysis in ms

   TH1* result(0);
   if(ob1 && ob1->InheritsFrom("TH2")){  // 2d rebin
      TH2* his1 = (TH2*)ob1;
      TString n1 = his1->GetName();
      TString t1 = his1->GetTitle();
      newname.Form("_rebinned_%d_%d",ngroup1,ngroup2);
      TString finalname=n1+newname;
      TString finaltitle=t1+newname;
      result = his1->Rebin2D(ngroup1,ngroup2,finalname);
      result->SetTitle(finaltitle);
   } else if(ob1 && ob1->InheritsFrom("TH1")){
      ngroup2=0;
      TH1* his1=(TH1*)ob1;
      TString n1=his1->GetName();
      TString t1=his1->GetTitle();
      newname.Form("_rebinned_%d",ngroup1);
      TString finalname = n1+newname;
      TString finaltitle = t1+newname;
      result = his1->Rebin(ngroup1,finalname);
      result->SetTitle(finaltitle);
   } else {
      std::cout <<"rebin could not get histogram "<<fullname1 << std::endl;
      return kFALSE;
   }

   result->SetDirectory(0);
   TString rname = go4->SaveToMemory("Rebinned", result, kTRUE);
   std::cout<< "Saved result histogram to " << rname.Data() <<std::endl;
   if(draw){
      ViewPanelHandle vpanel = go4->StartViewPanel();
      if(ngroup2 == 0){ // overlay 1D only
         go4->SetSuperimpose(vpanel,kTRUE);
         go4->DrawItem(fullname1, vpanel);
      }
      go4->DrawItem(rname, vpanel);
   }
   return kTRUE;
}
