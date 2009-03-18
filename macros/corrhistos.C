///////////////////////////////////////////////////////////////////
//////// Go4 GUI example script corrhistos.C
//        H.Essel, gsi, May 2006
// NOTE: to be run in Go4 GUI local command line or in ROOT CINT!
//       NEVER call this script in remote analysis process!!!
/////// Functionality:
// Creates a graph x-y with bin by bin contents of histogram of name1 and histogram of name2.
/////// Usage running in Go4 GUI:
// The draw flag switches if the results are displayed each time this makro is called
// if display is switched off, the result histogram is just updated in browser and existing displays
/////// Usage running in plain ROOT:
// There is one more argument: the name of a root file where the two histograms must be.
// The draw flag switches if the results are displayed each time this makro is called
// if display is switched off, result is lost
///////
#include "Riostream.h"

#ifdef __GO4MACRO__
// Get histograms from GO4 GUI
Bool_t corrhistos(const char* name1, const char* name2, Bool_t draw)
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
  TString fullname2 = go4->FindItem(name2);
  TObject* ob2=go4->GetObject(fullname2,1000);
  if(ob2 && ob2->InheritsFrom("TH1"))
    TH1* his2 = (TH1*)ob2;
  else
    {
      cout <<"corr could not get histogram "<<fullname2 << endl;
      return kFALSE;
    }
#else
  // Get histograms from file
Bool_t corrhistos(const char *file, const char* name1, const char* name2, Bool_t draw)
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
  TH1* his2=f->Get(name2);
  if(his2 == 0)
    {
      cout <<"corrhistos could not get histogram "<<name2 << " in file " << file << endl;
      return kFALSE;
    }
#endif
  TString n1=his1->GetName();
  TString n2=his2->GetName();
  TString t1=his1->GetName();
  TString t2=his2->GetName();
  TString finalname=n1+"-"+n2;
  TString finaltitle=t1+" x "+t2;
  Int_t nb1=his1->GetNbinsX();
  Int_t nb2=his2->GetNbinsX();
  Int_t n = nb1;
  if(nb1 > nb2) n = nb2;
  TArrayD *x = new TArrayD(n);
  TArrayD *y = new TArrayD(n);
  for(Int_t i=0;i<n;i++)
    {
      x->AddAt(his1->GetBinContent(i+1),i);
      y->AddAt(his2->GetBinContent(i+1),i);
    }
  result= new TGraph(n,x->GetArray(),y->GetArray());
  result->SetNameTitle(finalname,finaltitle);
  result->SetMarkerColor(4);
  result->SetMarkerSize(0.3);
  result->SetMarkerStyle(20);
  result->SetLineColor(1);

#ifdef __GO4MACRO__
  // save to Workspace folder Correlations
  graname=go4->SaveToMemory("Correlations",result,kTRUE);
  // returns full name needed for drawing
  cout << graname.Data() << endl;
  if(!draw) return kTRUE;
  ViewPanelHandle vpanel = go4->StartViewPanel();
  go4->DrawItem(graname, vpanel, "AP");
#else
  TCanvas *canv=new TCanvas(file,finaltitle,1100,800);
  result->Draw("AP");
  result->GetXaxis()->SetTitle("xxx");
  result->GetYaxis()->SetTitle("yyy");
  result->GetXaxis()->CenterTitle();
  result->GetYaxis()->CenterTitle();
  result->Draw("AP");
  canv->Update();
#endif
  return kTRUE;
}
