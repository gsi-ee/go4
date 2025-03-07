// $Id: TXXXAnlProc.cxx 478 2009-10-29 12:26:09Z linev $
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TXXXAnlProc.h"

#include "TH1.h"
#include "TGraph.h"

#include "TGo4Log.h"
#include "TGo4WinCond.h"
#include "TGo4Fitter.h"

#include "TXXXAnlEvent.h"
#include "TXXXUnpackEvent.h"
#include "TXXXParameter.h"
#include "TXXXCalibPar.h"

//***********************************************************
TXXXAnlProc::TXXXAnlProc() :
   TGo4EventProcessor()
{
}

//***********************************************************
// this one is used in TXXXAnlFact
TXXXAnlProc::TXXXAnlProc(const char *name) :
   TGo4EventProcessor(name)
{
   TGo4Log::Info("TXXXAnlProc: Create %s", name);
   //// init user analysis objects:
   fParam1 = (TXXXParameter *)  GetParameter("XXXPar1");
   fParam2 = (TXXXParameter *)  GetParameter("XXXPar2");
   fWinCon = (TGo4WinCond *)   GetAnalysisCondition("wincon1");

   fSum1     = MakeTH1('I', "Sum1", "Sum over 8 channels", 5000, 1., 5001.);
   fSum2     = MakeTH1('I', "Sum2", "Sum over 8 channels shift 1", 5000, 1., 5001.);
   fSum3     = MakeTH1('I', "Sum3", "Sum over 8 channels shift 2", 5000, 1., 5001.);
   fCaliSum1 = MakeTH1('I', "Sum1Calib", "Sum over 8 channels(keV)", 5000, 1., 20.);

   fFitSrc   = MakeTH1('I', "FitSource","Copy of fit data", 1000, 0., 1000.);
   fFitRes   = MakeTH1('I', "FitTarget","Copy of fit result", 1000, 0., 1000.);

   fCaligraph = (TGraph *) GetObject("Calibration");
   if (!fCaligraph) {
      fCaligraph = new TGraph;
      fCaligraph->SetName("Calibration");
      fCaligraph->SetMarkerStyle(3);
      AddObject(fCaligraph);
   }

   fFitter = (TGo4Fitter *) GetObject("Fitter");
   if (!fFitter) {
      fFitter = new TGo4Fitter("Fitter", TGo4Fitter::ff_chi_square, kTRUE);
      fFitter->AddH1("data", nullptr, kFALSE, 100., 1000.);
      fFitter->AddPolynomX("data", "Pol", 1);
      AddObject(fFitter);
   }

   fCalipar = (TXXXCalibPar *) GetParameter("CaliPar");
   if (!fCalipar) {
      // calibration parameter not yet existing, we set it up:
      fCalipar = new TXXXCalibPar("CaliPar",GetHistogram("Cr1Ch01"),fCaligraph);
      AddParameter(fCalipar);
   }

   fFitCounter = 0;
}

//***********************************************************
TXXXAnlProc::~TXXXAnlProc()
{
}

//-----------------------------------------------------------

Bool_t TXXXAnlProc::BuildEvent(TGo4EventElement *dest)
{
   auto inp_evt = static_cast<TXXXUnpackEvent *>(GetInputEvent());
   auto out_evt = static_cast<TXXXAnlEvent *>(dest);

   out_evt->SetValid(kFALSE);           // events are not stored until kTRUE is set
   if(!inp_evt || !inp_evt->IsValid())  // do not process unvalid event
      return kFALSE;
   out_evt->SetValid(kTRUE);       // events are not stored until kTRUE is set
   Int_t cnt = 0;
   TXXXUnpackEvent& ev=*inp_evt; // ref instead pointer for array syntax below
   for (Int_t cr = 1; cr < 3; cr++) {
      // loop over first filled crates 1 and 2
      for (Int_t ii = 0; ii < 4; ii++) {
         // get first channels of each crate
         TXXXModule* mod=dynamic_cast<TXXXModule*>( &ev[cr][ii]); // 2d array with composite event operator[]
         if(!mod) continue;
         Float_t val= mod->GetData();
         out_evt->frData[cnt] = val;
         if(val) fCaliSum1->Fill(fCalipar->Energy(val));
         cnt++;
      }
   }

   for (Int_t ii = 0; ii < 8; ii++)
      if (out_evt->frData[ii]) {
         if(fWinCon && fWinCon->Test(out_evt->frData[ii])) fSum1->Fill(out_evt->frData[ii]);
         if (fParam1) fSum2->Fill(out_evt->frData[ii] + fParam1->frP1);
         if (fParam2) fSum3->Fill(out_evt->frData[ii] + fParam2->frP1);
      }

   fFitCounter++;

   if ((fFitCounter % 500000 == 0) && fFitter) {
      TH1 *histo1 = GetHistogram("Crate1/Cr1Ch04");

      if (histo1) {
         fFitter->SetH1("data", histo1, kFALSE);
         fFitter->DoActions();

         fFitSrc->Reset();
         fFitSrc->SetBins(histo1->GetNbinsX(), histo1->GetXaxis()->GetXmin(), histo1->GetXaxis()->GetXmax());
         fFitSrc->Add(histo1);

         TH1 *histo2 = (TH1 *) fFitter->CreateDrawObject("FitResult", "data", kTRUE);
         fFitRes->Reset();
         if (histo2) {
            fFitRes->SetBins(histo2->GetNbinsX(), histo2->GetXaxis()->GetXmin(), histo2->GetXaxis()->GetXmax());
            fFitRes->Add(histo2);

            TString title = TString::Format("Result K=%5.3f B=%7.1f", fFitter->GetParValue("Pol_1.Ampl"), fFitter->GetParValue("Pol_0.Ampl"));
            fFitRes->SetTitle(title.Data());
            delete histo2;
         }
      }
   }

   return kTRUE;
}
