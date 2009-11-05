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
#include "Riostream.h"

#include "TGo4WinCond.h"
#include "TGo4Analysis.h"

#include "TXXXAnlEvent.h"
#include "TXXXUnpackEvent.h"
#include "TXXXParameter.h"
#include "TROOT.h"

//***********************************************************
TXXXAnlProc::TXXXAnlProc() :
   TGo4EventProcessor(),
   fSum1(0),fSum2(0),fSum3(0),
   fParam1(0),fWinCon(0)
{
}
//***********************************************************
// this one is used in TXXXAnlFact
TXXXAnlProc::TXXXAnlProc(const char* name) :
   TGo4EventProcessor(name)
{
   cout << "**** TXXXAnlProc: Create" << endl;
   //// init user analysis objects:
   fParam1 = (TXXXParameter*)  GetParameter("XXXParameter");
   // if unpack was enabled, parameters have been printed already
   if(TGo4Analysis::Instance()->GetAnalysisStep("Unpack")->IsProcessEnabled())
	    gROOT->ProcessLine(".x setparam.C(0)");
   else gROOT->ProcessLine(".x setparam.C(1)");

   if(fParam1->fbHisto){
// this one is created in TXXXAnalysis, because it is used in both steps
   fWinCon = (TGo4WinCond *) GetAnalysisCondition("wincon1");
   fWinCon->PrintCondition(true);
   fSum1     = MakeTH1('I', "Sum1", "Sum over 8 channels", 5000, 1., 5001.);
   fSum2     = MakeTH1('I', "Sum2", "Sum over 8 channels shift 1", 5000, 1., 5001.);
   fSum3     = MakeTH1('I', "Sum3", "Sum over 8 channels shift 2", 5000, 1., 5001.);
   }
}
//***********************************************************
TXXXAnlProc::~TXXXAnlProc()
{
	   cout << "**** TXXXAnlProc: Delete" << endl;
	   if(fParam1->fbHisto){
	   fWinCon->PrintCondition(true);
}}
//***********************************************************

//-----------------------------------------------------------

Bool_t TXXXAnlProc::BuildEvent(TGo4EventElement* dest)
{
   TXXXUnpackEvent* inp_evt  = (TXXXUnpackEvent*) GetInputEvent();
   TXXXAnlEvent* out_evt = (TXXXAnlEvent*) dest;

   out_evt->SetValid(kFALSE);       // events are not stored until kTRUE is set
   if((inp_evt==0) || !inp_evt->IsValid()) return kFALSE;    // do not process unvalid event
   out_evt->SetValid(kTRUE);       // events are not stored until kTRUE is set
   Int_t cnt(0);
   for(Int_t ii=0;ii<4;ii++) {
      out_evt->frData[cnt] = (Float_t)inp_evt->fiCrate1[ii];
      cnt++;
   }
   for(Int_t ii=0; ii<4; ii++) {
      out_evt->frData[cnt] = (Float_t)inp_evt->fiCrate2[ii];
      cnt++;
   }
   if(fParam1->fbHisto){ // histogramming
   for(Int_t ii=0;ii<8;ii++)
      if(out_evt->frData[ii]) {
         if(fWinCon->Test(out_evt->frData[ii])) fSum1->Fill(out_evt->frData[ii]);
         fSum2->Fill(out_evt->frData[ii]+fParam1->frP1);
         fSum3->Fill(out_evt->frData[ii]+fParam1->frP2);
      }
   }

   return kTRUE;
}
