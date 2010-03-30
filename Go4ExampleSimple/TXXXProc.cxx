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

#include "TXXXProc.h"

#include <stdlib.h>
#include "Riostream.h"

#include "TH1.h"
#include "TH2.h"
#include "TCutG.h"

#include "TGo4MbsEvent.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4CondArray.h"
#include "TGo4Picture.h"
#include "TGo4StepFactory.h"
#include "TGo4Analysis.h"
#include "TGo4Version.h"

#include "TXXXParam.h"

//***********************************************************
TXXXProc::TXXXProc() : TGo4EventProcessor()
{
}

//***********************************************************
TXXXProc::~TXXXProc()
{
   cout << "**** TXXXProc: Delete instance " << endl;
}

//***********************************************************
// this one is used in standard factory
TXXXProc::TXXXProc(const char* name) : TGo4EventProcessor(name)
{
   cout << "**** TXXXProc: Create instance " << name << endl;

   fParam = (TXXXParam*) MakeParameter("Par1", "TXXXParam");

   for(int i=0;i<8;i++) {
      fCr1Ch[i] = MakeTH1('I', Form("Crate1/Cr1Ch%02d",i+1), Form("Crate 1 channel %2d",i+1), 5000, 1., 5001.);
      fCr2Ch[i] = MakeTH1('I', Form("Crate2/Cr2Ch%02d",i+1), Form("Crate 2 channel %2d",i+1), 5000, 1., 5001.);
   }

   fCr1Ch1x2 = MakeTH2('I', "Cr1Ch1x2","Crate 1 channel 1x2", 200, 1., 5001., 200, 1., 5001.);
   fHis1 = MakeTH1('I', "His1","Condition histogram", 5000, 1., 5001.);
   fHis2 = MakeTH1('I', "His2","Condition histogram", 5000, 1., 5001.);
   fHis1gate = MakeTH1('I', "His1g","Gated histogram", 5000, 1., 5001.);
   fHis2gate = MakeTH1('I', "His2g","Gated histogram", 5000, 1., 5001.);

   fconHis1 = MakeWinCond("cHis1", 100,2000, "His1");
   fconHis2 = MakeWinCond("cHis2", 100,2000, "His2");

   Double_t cutpnts[3][2] = { {400, 800}, {700, 900}, {600, 1100} };
   fPolyCon = MakePolyCond("polycon", 3, cutpnts);

   fcondSet = GetPicture("condSet");
   if (fcondSet==0) {
      // in the upper two pads, the condition limits can be set,
      // in the lower two pads, the resulting histograms are shown
      fcondSet = new TGo4Picture("condSet","Set conditions");
      fcondSet->SetDivision(2,2);
      fcondSet->Pic(0,0)->AddObject(fHis1);
      fcondSet->Pic(0,1)->AddObject(fHis2);
      fcondSet->Pic(0,0)->AddCondition(fconHis1);
      fcondSet->Pic(0,1)->AddCondition(fconHis2);
      fcondSet->Pic(1,0)->AddObject(fHis1gate);
      fcondSet->Pic(1,1)->AddObject(fHis2gate);
      fcondSet->Pic(1,0)->SetFillAtt(4, 1001); // solid
      fcondSet->Pic(1,0)->SetLineAtt(4,1,1);
      fcondSet->Pic(1,1)->SetFillAtt(9, 1001); // solid
      fcondSet->Pic(1,1)->SetLineAtt(9,1,1);
      AddPicture(fcondSet);
   }
}

//-----------------------------------------------------------
// event function
Bool_t TXXXProc::BuildEvent(TGo4EventElement*)
{  // called by framework. We dont fill any output event here at all

   TGo4MbsEvent* evnt = (TGo4MbsEvent* ) GetInputEvent();
   if(evnt == 0) {
      cout << "AnlProc: no input event !"<< endl;
      return kFALSE;
   }
   if(evnt->GetTrigger() > 11) {
      cout << "**** TXXXProc: Skip trigger event"<<endl;
      return kFALSE;
   }
   // first we fill the arrays fCrate1,2 with data from MBS source
   // we have up to two subevents, crate 1 and 2
   // Note that one has to loop over all subevents and select them by
   // crate number:   psubevt->GetSubcrate(),
   // procid:         psubevt->GetProcid(),
   // and/or control: psubevt->GetControl()
   // here we use only crate number

   Float_t Crate1[8];
   Float_t Crate2[8];
   for(Int_t i = 0; i<8; i++) {
      Crate1[i] = 0.;
      Crate2[i] = 0.;
   }

   evnt->ResetIterator();
   TGo4MbsSubEvent *psubevt(0);
   while((psubevt = evnt->NextSubEvent()) != 0) { // loop over subevents
      Int_t * pdata = psubevt->GetDataField();
      Int_t lwords = psubevt->GetIntLen();
      if(lwords > 8) lwords = 8; // take only first 8 lwords

      if(psubevt->GetSubcrate() == 1)
         for(Int_t i = 0; i<lwords; i++)
            Crate1[i] = (Float_t)*pdata++;

      if(psubevt->GetSubcrate() == 2)
         for(Int_t i = 0; i<lwords; i++)
            Crate2[i] = (Float_t)*pdata++;
   }

   // now we fill histograms from the arrays
   if (fParam->fillRaw)
      for(Int_t i = 0; i<8; i++) {
         fCr1Ch[i]->Fill(Crate1[i]);
         fCr2Ch[i]->Fill(Crate2[i]);
      }
   Float_t value1 = Crate1[0];
   Float_t value2 = Crate1[1];
   fHis1->Fill(value1); // fill histograms without gate
   fHis2->Fill(value2);
   if(fconHis1->Test(value1))fHis1gate->Fill(value1); //fill histograms with gate
   if(fconHis2->Test(value2))fHis2gate->Fill(value2);
   if(fPolyCon->Test(value1,value2)) fCr1Ch1x2->Fill(value1,value2);
   return kTRUE;
}
