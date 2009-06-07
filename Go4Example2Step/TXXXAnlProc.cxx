#include "TXXXAnlProc.h"

#include <math.h>

#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "Riostream.h"

#include "TGo4WinCond.h"
#include "TGo4Fitter.h"

#include "TXXXAnlEvent.h"
#include "TXXXUnpackEvent.h"
#include "TXXXParameter.h"
#include "TXXXCalibPar.h"

//***********************************************************
TXXXAnlProc::TXXXAnlProc()
  : TGo4EventProcessor(),fSum1(0),fSum2(0),fSum3(0),
    fInput(0),fParam1(0),fParam2(0),fWinCon(0)
{
}
//***********************************************************
// this one is used in TXXXAnlFact
TXXXAnlProc::TXXXAnlProc(const char* name) :
  TGo4EventProcessor(name)
{
  cout << "**** TXXXAnlProc: Create" << endl;
  //// init user analysis objects:
  fParam1 = (TXXXParameter*)  GetParameter("XXXPar1");
  fParam2 = (TXXXParameter*)  GetParameter("XXXPar2");
  fWinCon = (TGo4WinCond *)   GetAnalysisCondition("wincon1");

  // we must check, if the histograms have been restored from auto-save file
  if(GetHistogram("Sum1")==0)
    { // no auto-save file, create new
      fSum1    = new TH1I ("Sum1", "Sum over 8 channels",5000,1,5001);
      fSum2    = new TH1I ("Sum2", "Sum over 8 channels shift 1",5000,1,5001);
      fSum3    = new TH1I ("Sum3", "Sum over 8 channels shift 2",5000,1,5001);
      fCaliSum1= new TH1I ("Sum1Calib", "Sum over 8 channels(keV)",5000,1,20);
      AddHistogram(fSum1);
      AddHistogram(fSum2);
      AddHistogram(fSum3);
      AddHistogram(fCaliSum1);
      fCaligraph=new TGraph;
      fCaligraph->SetName("Calibration");
      fCaligraph->SetMarkerStyle(3);
      AddObject(fCaligraph);
      // test of multiple graphs:
      fxGraph1=new TGraph;
      fxGraph1->SetName("Testgraph1");
      fxGraph1->SetMarkerStyle(5);
      //AddObject(fxGraph1);
      for(Int_t i=0;i<50;++i)
         {
             fxGraph1->SetPoint(i,i,30*sin((Double_t)(i)/10));
         }
      fxGraph2=new TGraph;
      fxGraph2->SetName("Testgraph2");
      fxGraph2->SetMarkerStyle(4);
      //AddObject(fxGraph2);
      for(Int_t i=0;i<50;++i)
         {
             fxGraph2->SetPoint(i,i,30*cos((Double_t)(i)/10));
         }
      fxMultiGraph=new TMultiGraph("MultiTest","This is a test multigraph");
      fxMultiGraph->Add(fxGraph1);
      fxMultiGraph->Add(fxGraph2);
      AddObject(fxMultiGraph);

      fFitter = new TGo4Fitter("Fitter", TGo4Fitter::ff_chi_square, kTRUE);
      fFitter->AddH1("data", 0, kFALSE, 100., 1000.);
      fFitter->AddPolynomX("data", "Pol", 1);
      AddObject(fFitter);

      fFitSrc = new TH1I("FitSource","Copy of fit data", 1000, 0, 1000);
      fFitRes = new TH1I("FitTarget","Copy of fit result", 1000, 0, 1000);
      AddObject(fFitSrc);
      AddObject(fFitRes);
    }
  else
    { // restored from auto-save file, get pointers
      fSum1    =(TH1I*)GetHistogram("Sum1");
      fSum2    =(TH1I*)GetHistogram("Sum2");
      fSum3    =(TH1I*)GetHistogram("Sum3");
      fCaliSum1=(TH1I*)GetHistogram("Sum1Calib");
      fCaligraph= (TGraph*)GetObject("Calibration");
      fFitter = (TGo4Fitter*)GetObject("Fitter");
      fFitSrc = (TH1I*)GetObject("FitSource");
      fFitRes = (TH1I*)GetObject("FitTarget");
      cout << "AnlProc: Restored histograms from autosave" << endl;
    }
  if(GetParameter("CaliPar")==0)
    {
      // calibration parameter not yet existing, we set it up:
      fCalipar=new TXXXCalibPar("CaliPar",GetHistogram("Cr1Ch01"),fCaligraph);
      AddParameter(fCalipar);
    }
  else
    {
      // use calibration parameter from autosave or previous run:
      fCalipar= (TXXXCalibPar*) GetParameter("CaliPar");
    }

  fFitCounter = 0;
}
//***********************************************************
TXXXAnlProc::~TXXXAnlProc()
{
}
//***********************************************************

//-----------------------------------------------------------
void TXXXAnlProc::XXXEventAnalysis(TXXXAnlEvent* poutevt)
{
  Int_t ii,i;
  fInput  = (TXXXUnpackEvent*) GetInputEvent();

  poutevt->SetValid(kTRUE);       // events are not stored until kTRUE is set
  i=0;
  for(ii=0;ii<4;ii++)
    {
      poutevt->frData[i]=(Float_t)fInput->fiCrate1[ii];
      if(fInput->fiCrate1[ii]) fCaliSum1->Fill(fCalipar->Energy(fInput->fiCrate1[ii]));
      i++;
    }
  for(ii=0;ii<4;ii++)
    {
      poutevt->frData[i]=(Float_t)fInput->fiCrate2[ii];
      if(fInput->fiCrate2[ii]) fCaliSum1->Fill(fCalipar->Energy(fInput->fiCrate2[ii]));
      i++;
    }
  for(ii=0;ii<8;ii++)
    {
      if(poutevt->frData[ii])
   {
     if(fWinCon->Test(poutevt->frData[ii])) fSum1->Fill(poutevt->frData[ii]);
     fSum2->Fill(poutevt->frData[ii]+fParam1->frP1);
     fSum3->Fill(poutevt->frData[ii]+fParam2->frP1);
   }
    }

  fFitCounter++;

  if ((fFitCounter % 5000000 == 0) && (fFitter!=0)) {
     TH1* histo1 = GetHistogram("Crate1/Cr1Ch04");

     if (histo1!=0) {
        fFitter->SetH1("data", histo1, kFALSE);
        fFitter->DoActions();
//        cout << "K = " << fFitter->GetParValue("Pol_1.Ampl") << " B = " << fFitter->GetParValue("Pol_0.Ampl") << endl;
//        cout << "FF = " << fFitter->GetResultFF() << " NDF = " << fFitter->GetResultNDF() << endl;

        fFitSrc->Reset();
        fFitSrc->SetBins(histo1->GetNbinsX(), histo1->GetXaxis()->GetXmin(), histo1->GetXaxis()->GetXmax());
        fFitSrc->Add(histo1);

        TH1* histo2 = (TH1*) fFitter->CreateDrawObject("FitResult", "data", kTRUE);
        fFitRes->Reset();
        if (histo2!=0) {
           fFitRes->SetBins(histo2->GetNbinsX(), histo2->GetXaxis()->GetXmin(), histo2->GetXaxis()->GetXmax());
           fFitRes->Add(histo2);
           fFitRes->SetTitle(Form("Result K=%5.3f B=%7.1f", fFitter->GetParValue("Pol_1.Ampl"), fFitter->GetParValue("Pol_0.Ampl")));
           delete histo2;
        }
     }
  }


} // BuildCalEvent
