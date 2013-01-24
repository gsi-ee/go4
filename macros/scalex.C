#include "Riostream.h"

///////////////////////////////////////////////////////////////////
//////// Go4 GUI example script scalex.C
//          J.Adamczewski, gsi, Nov 2006
// NOTE: to be run in Go4 GUI local command line only!
//       NEVER call this script in remote analysis process!!!
/////// Functionality:
// rescales x-axis of histogram name1 by linear factors at,a0
/////// Usage:
// in Go4 GUI command line, load macro:
// .L scalex.C
// call macro function with full histogram name
//  (drag and drop histogram icon from browser to macro line):
// scalex("Analysis/Histograms/Sum1",0.00341,1.91);
// The draw flag switches if the results are displayed each time this makro is called
// if display is switched off, the result histogram is just updated in browser and existing displays
///////
Bool_t scalex(const char* name1, Double_t a1 = 1, Double_t a0= 0, Bool_t draw = kTRUE)
{
   if(TGo4AbstractInterface::Instance()==0 || go4!=TGo4AbstractInterface::Instance()) {
      std::cout <<"FATAL: Go4 gui macro executed outside Go4 GUI!! returning." << std::endl;
      return kFALSE;
   }

   TString fullname1 = go4->FindItem(name1);
   TObject* ob1=go4->GetObject(fullname1,1000); // 1000=timeout to get object from analysis in ms
   TH1* his1(0);
   if(ob1 && ob1->InheritsFrom("TH1"))
      his1 = (TH1*)ob1;
   if(his1==0) {
      std::cout <<"rebin could not get histogram "<<fullname1 << std::endl;
      return kFALSE;
   }
   if(his1->InheritsFrom("TH2") || his1->InheritsFrom("TH3")) {
      std::cout <<"can not scale multi dim histogram "<<fullname1 << std::endl;
      return kFALSE;
   }
   TString n1 = his1->GetName();
   TString t1 = his1->GetTitle();
   TString soper;
   soper.Form("_scaled_a1:%f_a0:%f",a1,a0);
   TString finalname = n1+soper;
   TString finaltitle = t1+soper;
   Int_t nbins = his1->GetNbinsX();
   std::cout <<"rescaling histogram of " << nbins << " bins..." << std::endl;

   Double_t* binarray = new Double_t[nbins+1];
   for(Int_t i=0; i<=nbins; ++i)
      binarray[i] = a1*(his1->GetXaxis()->GetBinUpEdge(i)) + a0;

   TH1* result = new TH1I(finalname, finaltitle, nbins, binarray);
   for(Int_t i=0; i<nbins; ++i)
      result->SetBinContent(i,his1->GetBinContent(i)); // copy contents to scaled bin
   delete[] binarray;


   result->SetDirectory(0);
   TString rname = go4->SaveToMemory("Scaled", result, kTRUE);
   std::cout<< "Saved result histogram to " << rname.Data() <<std::endl;
   if(draw) {
      ViewPanelHandle vpanel = go4->StartViewPanel();
      go4->DrawItem(rname, vpanel);
   }
   return kTRUE;
}
