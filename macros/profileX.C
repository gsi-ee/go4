#include "Riostream.h"

///////////////////////////////////////////////////////////////////
//////// Go4 GUI example script projectionX.C
//          J.Adamczewski, gsi, May 2006
// NOTE: to be run in Go4 GUI local command line only!
//       NEVER call this script in remote analysis process!!!
/////// Functionality:
// create profile of histogram name1 along x axis
/////// Usage:
// parameters of root TH2::ProfileX method may be specified:
//   the range of profile concerning y bins(firstybin, lastybin);
// The draw flag switches if the results are displayed each time this makro is called
// if display is switched off, the result histogram is just updated in browser and existing displays
///////
Bool_t profileX(const char* name1, Int_t firstybin, Int_t lastybin, Bool_t draw)
{
   if(TGo4AbstractInterface::Instance()==0 || go4!=TGo4AbstractInterface::Instance()) {
      std::cout <<"FATAL: Go4 gui macro executed outside Go4 GUI!! returning." << std::endl;
      return kFALSE;
   }
   TString fullname1 = go4->FindItem(name1);
   TObject* ob1 = go4->GetObject(fullname1,1000); // 1000=timeout to get object from analysis in ms
   TH2 *his1(0);
   if(ob1 && ob1->InheritsFrom("TH2"))
      his1 = (TH2*) ob1;
   if(his1==0) {
      std::cout <<"profileX could not get 2d histogram "<<fullname1 << std::endl;
      return kFALSE;
   }

   TString n1 = his1->GetName();
   TString t1 = his1->GetTitle();
   TString soper;
   soper.Form("(ylo=%d, yup=%d)",firstybin,lastybin);
   TString finalname = TString("X-Profile of ")+n1;
   TString finaltitle = TString("X-Prof.") + soper+" of "+t1;
   TH1* result = his1->ProfileX(finalname.Data(),firstybin,lastybin);
   result->SetTitle(finaltitle);
   result->SetDirectory(0);
   TString rname = go4->SaveToMemory("Profiles", result, kTRUE);
   std::cout<< "Saved result histogram to " << rname.Data() <<std::endl;
   if(draw) {
      ViewPanelHandle panel1 = go4->StartViewPanel();
      go4->DivideViewPanel(panel1, 1, 2);
      go4->SelectPad(panel1, 1);
      go4->DrawItem(fullname1, panel1,"COLZ");
      go4->SelectPad(panel1, 2);
      go4->DrawItem(rname, panel1);
   }
   return kTRUE;
}
