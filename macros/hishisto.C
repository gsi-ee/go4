///////////////////////////////////////////////////////////////////
//////// Go4 GUI example script hishisto.C
//          H.Essel, gsi, May 2006
// NOTE: to be run in Go4 GUI local command line or in ROOT CINT!
//       NEVER call this script in remote analysis process!!!
/////// Functionality:
// Creates a histogram with requested number of bins and fills it with bin contents of histogram name1.
/////// Usage running in Go4 GUI:
// The draw flag switches if the results are displayed each time this makro is called
// if display is switched off, the result histogram is just updated in browser and existing displays
/////// Usage running in plain ROOT:
// There is one more argument: the name of a root file where the histogram must be.
// The draw flag switches if the results are displayed each time this makro is called
// if display is switched off, result is lost
///////
#include "Riostream.h"
using namespace std;


#ifdef __GO4MACRO__
// Get histograms from GO4 GUI
Bool_t hishisto(const char* name1, Int_t bins, Bool_t draw)
{

  if(TGo4AbstractInterface::Instance()==0 || go4!=TGo4AbstractInterface::Instance())
    {
      cout <<"FATAL: Go4 gui macro executed outside Go4 GUI!! returning." << endl;
      return;
    }
  TString fullname1 = go4->FindItem(name1);
  TObject* ob1=go4->GetObject(fullname1,1000);
  if(ob1 && ob1->InheritsFrom("TH1"))
    TH1* his1 = (TH1*)ob1;
  else
    {
      cout <<"corr could not get histogram "<<fullname1 << endl;
      return kFALSE;
    }

#else
  // Get histograms from file
Bool_t hishisto(const char *file, const char* name1, Int_t bins, Bool_t draw)
{
  TFile *f=TFile::Open(file,"r");
  if(f == 0)
    {
      cout <<"corrhistos could not open file " << file << endl;
      return kFALSE;
    }
  TH1* his1=f->Get(name1);
  if(his1 == 0)
    {
      cout <<"corrhistos could not get histogram "<<name1 << " in file " << file << endl;
      return kFALSE;
    }
#endif
  TString n1=his1->GetName();
  TString t1=his1->GetTitle();
  TString finalname=n1+"-stat";
  TString axistitle=n1+" content";
  TString finaltitle=t1+" statistics";
  Double_t max=his1->GetMaximum();
  Double_t min=his1->GetMinimum();
  cout << min << " " << max << endl;
  if(max > 0) max = 1.1*max;
  else        max = 0.9*max;
  if(min > 0) min = 0.9*min;
  else        min = 1.1*min;
  stat=new TH1I(finalname,finaltitle,bins,min,max);
  stat->GetXaxis()->SetTitle(axistitle);
  stat->GetYaxis()->SetTitle("Counts");
  TGaxis::SetMaxDigits(2);
  stat->GetXaxis()->SetNoExponent(kTRUE);

  Int_t nb1=his1->GetNbinsX();
  for(Int_t i=1;i<=nb1;i++)
    {
      stat->Fill(his1->GetBinContent(i));
    }

#ifdef __GO4MACRO__
  // save to Workspace folder Statistics
  graname=go4->SaveToMemory("Statistics",stat,kTRUE);
  cout << graname.Data() << endl;
  if(!draw) return kTRUE;
  ViewPanelHandle vpanel = go4->StartViewPanel();
  go4->DrawItem(graname, vpanel, "");
#else
  TCanvas *canv=new TCanvas(file,finaltitle,1100,800);
  stat->Draw();
  canv->Update();
#endif
  return kTRUE;
}
