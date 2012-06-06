#include "Riostream.h"
using namespace std;


///////////////////////////////////////////////////////////////////
//////// Go4 GUI example script projectionY.C
//         J.Adamczewski, gsi, May 2006
// NOTE: to be run in Go4 GUI local command line only!
//       NEVER call this script in remote analysis process!!!
/////// Functionality:
// create projection of histogram name1 along y axis
/////// Usage:
// parameters of root TH2::ProjectionY method may be specified:
//   the range of projection concerning x bins(firstxbin, lastxbin);
// The draw flag switches if the results are displayed each time this makro is called
// if display is switched off, the result histogram is just updated in browser and existing displays
///////
Bool_t projectionY(const char* name1, const char* polyname, Int_t firstxbin, Int_t lastxbin, Bool_t draw)
{
if(TGo4AbstractInterface::Instance()==0 || go4!=TGo4AbstractInterface::Instance())
   {
      cout <<"FATAL: Go4 gui macro executed outside Go4 GUI!! returning." << endl;
      return;
   }
TString fullname1 = go4->FindItem(name1);
TObject* ob1=go4->GetObject(fullname1,1000); // 1000=timeout to get object from analysis in ms
TH2* his1=0;
if(ob1 && ob1->InheritsFrom("TH2"))   his1 = (TH2*) ob1;
if(his1==0)
   {
      cout <<"projectionY could not get 2d histogram "<<fullname1 << endl;
      return kFALSE;
   }
TGo4PolyCond* poly=0;
if(strlen(polyname)>0){
TString fullname2 = go4->FindItem(polyname);
TObject* ob2=go4->GetObject(fullname2,1000); // 1000=timeout to get object from analysis in ms
if(ob2 && ob2->InheritsFrom("TGo4Condition"))   poly = (TGo4PolyCond*) ob2;
if(poly==0)
   {
      cout <<"projectionX could not get polygon condition "<<polyname << endl;
      return kFALSE;
   }
}
TString n1=his1->GetName();
TString t1=his1->GetTitle();
TString oper;
oper.Form("(xlo=%d, xup=%d)",firstxbin,lastxbin);
TString finalname="Y-Projection of "+n1;
TString finaltitle="Y-Proj."+oper+" of "+t1;
TString options("");
if(poly){
	TCutG *newcut = poly->GetCut(kFALSE); // reference only
	TCutG *cut = newcut->Clone("tempcut");  // therefore create clone
	options.Form("[%s]",cut->GetName());
}
TH1* result=his1->ProjectionY(finalname.Data(),firstxbin,lastxbin);
if(poly) delete cut; // clone
result->SetTitle(finaltitle);
result->SetDirectory(0);
rname = go4->SaveToMemory("Projections", result, kTRUE);
cout<< "Saved result histogram to " << rname.Data() <<endl;
if(!draw) return kTRUE;
ViewPanelHandle panel1 = go4->StartViewPanel();
go4->DivideViewPanel(panel1, 2, 1);
go4->SelectPad(panel1, 2);
go4->DrawItem(fullname1, panel1,"COLZ");
go4->SelectPad(panel1, 1);
go4->DrawItem(rname, panel1);
return kTRUE;
}
