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
TXXXAnlProc::TXXXAnlProc() :
   TGo4EventProcessor(),
   fSum1(0),fSum2(0),fSum3(0),
   fParam1(0),fParam2(0),fWinCon(0)
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

   fSum1     = MakeH1('I', "Sum1", "Sum over 8 channels", 5000, 1., 5001.);
   fSum2     = MakeH1('I', "Sum2", "Sum over 8 channels shift 1", 5000, 1., 5001.);
   fSum3     = MakeH1('I', "Sum3", "Sum over 8 channels shift 2", 5000, 1., 5001.);
   fCaliSum1 = MakeH1('I', "Sum1Calib", "Sum over 8 channels(keV)", 5000, 1., 20.);

   fFitSrc   = MakeH1('I', "FitSource","Copy of fit data", 1000, 0., 1000.);
   fFitRes   = MakeH1('I', "FitTarget","Copy of fit result", 1000, 0., 1000.);

   fCaligraph = (TGraph*) GetObject("Calibration");
   if (fCaligraph==0) {
      fCaligraph=new TGraph;
      fCaligraph->SetName("Calibration");
      fCaligraph->SetMarkerStyle(3);
      AddObject(fCaligraph);
   }

   fFitter = (TGo4Fitter*)GetObject("Fitter");
   if (fFitter==0) {
      fFitter = new TGo4Fitter("Fitter", TGo4Fitter::ff_chi_square, kTRUE);
      fFitter->AddH1("data", 0, kFALSE, 100., 1000.);
      fFitter->AddPolynomX("data", "Pol", 1);
      AddObject(fFitter);
   }

   fCalipar = (TXXXCalibPar*) GetParameter("CaliPar");
   if (fCalipar==0) {
      // calibration parameter not yet existing, we set it up:
      fCalipar=new TXXXCalibPar("CaliPar",GetHistogram("Cr1Ch01"),fCaligraph);
      AddParameter(fCalipar);
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
   TXXXUnpackEvent* fInput  = (TXXXUnpackEvent*) GetInputEvent();

   poutevt->SetValid(kTRUE);       // events are not stored until kTRUE is set
   Int_t cnt(0);
   for(Int_t ii=0;ii<4;ii++) {
      poutevt->frData[cnt] = (Float_t)fInput->fiCrate1[ii];
      if(fInput->fiCrate1[ii]) fCaliSum1->Fill(fCalipar->Energy(fInput->fiCrate1[ii]));
      cnt++;
   }
   for(Int_t ii=0; ii<4; ii++) {
      poutevt->frData[cnt]=(Float_t)fInput->fiCrate2[ii];
      if(fInput->fiCrate2[ii]) fCaliSum1->Fill(fCalipar->Energy(fInput->fiCrate2[ii]));
      cnt++;
   }
   for(Int_t ii=0;ii<8;ii++)
      if(poutevt->frData[ii]) {
         if(fWinCon->Test(poutevt->frData[ii])) fSum1->Fill(poutevt->frData[ii]);
         fSum2->Fill(poutevt->frData[ii]+fParam1->frP1);
         fSum3->Fill(poutevt->frData[ii]+fParam2->frP1);
      }

   fFitCounter++;

   if ((fFitCounter % 500000 == 0) && (fFitter!=0)) {
      TH1* histo1 = GetHistogram("Crate1/Cr1Ch04");

      if (histo1!=0) {
         fFitter->SetH1("data", histo1, kFALSE);
         fFitter->DoActions();
         // cout << "K = " << fFitter->GetParValue("Pol_1.Ampl") << " B = " << fFitter->GetParValue("Pol_0.Ampl") << endl;
         // cout << "FF = " << fFitter->GetResultFF() << " NDF = " << fFitter->GetResultNDF() << endl;

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
