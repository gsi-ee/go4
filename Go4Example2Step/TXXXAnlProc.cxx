// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TXXXAnlProc.h"

#include <math.h>

#include "TH1.h"
#include "TH2.h"
#include "go4iostream.h"
#include "TROOT.h"

#include "TGo4WinCond.h"
#include "TGo4Analysis.h"

#include "TXXXAnlEvent.h"
#include "TXXXUnpackEvent.h"
#include "TXXXParameter.h"

//-----------------------------------------------------------
TXXXAnlProc::TXXXAnlProc() :
   TGo4EventProcessor(),
   fSum1(0), fSum2(0), fSum3(0),
   fParam(0), fWinCon(0)
{
}

//-----------------------------------------------------------
TXXXAnlProc::TXXXAnlProc(const char* name) :
   TGo4EventProcessor(name),
   fSum1(0), fSum2(0), fSum3(0),
   fParam(0), fWinCon(0)
{
   TGo4Log::Info("TXXXAnlProc: Create");
   //// init user analysis objects:

   // here already exisitng parameter will be returned
   // one not need to specify macro here - it is already executed in first step
   fParam = (TXXXParameter*) MakeParameter("XXXParameter", "TXXXParameter");

   // this one is created in TXXXAnalysis, because it is used in both steps
   fWinCon = (TGo4WinCond *) GetAnalysisCondition("wincon1", "TGo4WinCond");
   if (fWinCon) fWinCon->PrintCondition(true);

   if(fParam->fbHisto) {
      fSum1     = MakeTH1('I', "Sum1", "Sum over 8 channels", 5000, 1., 5001.);
      fSum2     = MakeTH1('I', "Sum2", "Sum over 8 channels shift 1", 5000, 1., 5001.);
      fSum3     = MakeTH1('I', "Sum3", "Sum over 8 channels shift 2", 5000, 1., 5001.);
   }
}
//-----------------------------------------------------------
TXXXAnlProc::~TXXXAnlProc()
{
   TGo4Log::Info("TXXXAnlProc: Delete");
   if(fParam->fbHisto){
      if (fWinCon) fWinCon->PrintCondition(true);
   }
}
//-----------------------------------------------------------
Bool_t TXXXAnlProc::BuildEvent(TGo4EventElement* dest)
{
	Bool_t isValid=kFALSE; // validity of output event

   TXXXUnpackEvent* inp_evt  = (TXXXUnpackEvent*) GetInputEvent();
   TXXXAnlEvent* out_evt = (TXXXAnlEvent*) dest;

   // see comments in UnpackProc
   if((inp_evt==0) || !inp_evt->IsValid()){ // input invalid
	  out_evt->SetValid(isValid); // invalid
	  return isValid; // must be same is for SetValid
   }
   isValid=kTRUE;

   Int_t cnt(0);
   for(Int_t ii=0;ii<XXX_NUM_CHAN/2;ii++)
      out_evt->frData[cnt++] = inp_evt->fiCrate1[ii];

   for(Int_t ii=0; ii<XXX_NUM_CHAN/2; ii++)
      out_evt->frData[cnt++] = inp_evt->fiCrate2[ii];

   if(fParam->fbHisto) { // histogramming
      for(Int_t ii=0;ii<XXX_NUM_CHAN;ii++)
         if(out_evt->frData[ii]) {
            if(fWinCon && fWinCon->Test(out_evt->frData[ii])) fSum1->Fill(out_evt->frData[ii]);
            fSum2->Fill(out_evt->frData[ii] + fParam->frP1);
            fSum3->Fill(out_evt->frData[ii] + fParam->frP2);
         }
   }

   // see comments in UnpackProc
   out_evt->SetValid(isValid);
   return isValid;
}
