// $Id$
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

#include "TXXXProc.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TCutG.h"

#include "TGo4Log.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4CondArray.h"
#include "TGo4Picture.h"
#include "TGo4MbsEvent.h"

#include "TXXXControl.h"
#include "TXXXEvent.h"

//***********************************************************
TXXXProc::TXXXProc() : TGo4EventProcessor()
{
   TGo4Log::Info("TXXXProc: Create instance");
}

//***********************************************************
TXXXProc::~TXXXProc()
{
   TGo4Log::Info("TXXXProc: Delete instance");
}

//***********************************************************
// this one is used in standard factory
TXXXProc::TXXXProc(const char *name) : TGo4EventProcessor(name)
{
   TGo4Log::Info("TXXXProc: Create instance %s", name);

   //// init user analysis objects:

   // Parameters are created in analysis, therefore just take it out
   fControl = (TXXXControl *) GetParameter("Control");

   // This example analysis allows for en-disabling the histogram filling.
   // Macro histofill.C sets histogram fill status in parameter "Control".
   // This macro histofill.C, not the auto save file, will set status.
   ExecuteScript("histofill.C");
   fControl->Print();

   TGo4Log::Info("TXXXProc: Produce histograms");
   // Creation of histograms (or take them from autosave)
   for (int i = 0; i < 8; i++) {
      TString hname = TString::Format("Crate1/Cr1Ch%02d", i + 1);
      TString htitle = TString::Format("Crate 1 channel %2d", i+1);
      fCr1Ch[i] = MakeTH1('I', hname.Data(), htitle.Data(), 5000, 1., 5001.);
      hname = TString::Format("Crate2/Cr2Ch%02d",i+1);
      htitle = TString::Format("Crate 2 channel %2d",i+1);
      fCr2Ch[i] = MakeTH1('I', hname.Data(), htitle.Data(), 5000, 1., 5001.);
   }

   fCr1Ch1x2 = MakeTH2('I', "Cr1Ch1x2","Crate 1 channel 1x2", 200, 1., 5001., 200, 1., 5001.);
   fHis1 = MakeTH1('I', "His1","Condition histogram", 5000, 1., 5001.);
   fHis2 = MakeTH1('I', "His2","Condition histogram", 5000, 1., 5001.);
   fHis1gate = MakeTH1('I', "His1g","Gated histogram", 5000, 1., 5001.);
   fHis2gate = MakeTH1('I', "His2g","Gated histogram", 5000, 1., 5001.);

   TGo4Log::Info("TXXXProc: Produce conditions");
   fconHis1 = MakeWinCond("cHis1", 100, 2000, "His1");
   fconHis2 = MakeWinCond("cHis2", 100, 2000, "His2");

   Double_t cutpnts[3][2] = { {400, 800}, {700, 900}, {600, 1100} };
   fPolyCon = MakePolyCond("polycon", 3, cutpnts);

   fConArr = (TGo4CondArray*)GetAnalysisCondition("polyconar");
   if(!fConArr) {
      // This is example how to create condition array
      TGo4Log::Info("TXXXProc: Create condition array");
      Double_t xvalues[4] = { 1000, 2000, 1500, 1000 };
      Double_t yvalues[4] = { 1000, 1000, 3000, 1000 };
      TCutG *mycut = new TCutG("cut2", 4, xvalues, yvalues);
      fConArr = new TGo4CondArray("polyconar",4,"TGo4PolyCond");
      fConArr->SetValues(mycut);
      fConArr->Disable(true);   // means: condition check always returns true
      delete mycut; // mycat has been copied into the conditions
      AddAnalysisCondition(fConArr);
   } else {
      TGo4Log::Info("TXXXProc: Restore condition array from autosave");
      fConArr->ResetCounts();
   }
   // connect histograms to conditions. will be drawn when condition is edited.
   ((*fConArr)[0])->Enable();
   ((*fConArr)[1])->Enable(); // 2 and 3 remain disabled

   TGo4Log::Info("TXXXProc: Produce pictures");
   fcondSet = GetPicture("condSet");
   if (!fcondSet) {
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

   fPicture = GetPicture("Picture");
   if (!fPicture) {
      fPicture = new TGo4Picture("Picture","Picture example");
      fPicture->SetLinesDivision(3, 2,3,1);
      fPicture->LPic(0,0)->AddObject(fCr1Ch[0]);
      fPicture->LPic(0,0)->SetFillAtt(5, 3001); // pattern
      fPicture->LPic(0,0)->SetLineAtt(5,1,1);
      fPicture->LPic(0,1)->AddObject(fCr1Ch[1]);
      fPicture->LPic(0,1)->SetFillAtt(4, 3001); // pattern
      fPicture->LPic(0,1)->SetLineAtt(4,1,1);
      fPicture->LPic(1,0)->AddObject(fCr1Ch[2]);
      fPicture->LPic(1,0)->SetFillAtt(6, 1001); // solid
      fPicture->LPic(1,0)->SetLineAtt(6,1,1);
      fPicture->LPic(1,1)->AddObject(fCr1Ch[3]);
      fPicture->LPic(1,1)->SetFillAtt(7, 1001); // solid
      fPicture->LPic(1,1)->SetLineAtt(7,1,1);
      fPicture->LPic(1,2)->AddObject(fCr1Ch[4]);
      fPicture->LPic(3,0)->AddObject(fCr1Ch1x2);
      fPicture->LPic(3,0)->SetDrawOption("CONT");
      AddPicture(fPicture);
   }

   fProfile = dynamic_cast<TProfile *>(GetObject("profile"));
   if (!fProfile) {
      fProfile = new TProfile("profile","Example of TProfile usage", 5000, 1., 5001., 1., 5001.);
      fProfile->SetDirectory(nullptr);
      AddObject(fProfile);
   }

   fProfile2D = dynamic_cast<TProfile2D *>(GetObject("profile2d"));
   if (!fProfile2D) {
      fProfile2D = new TProfile2D("profile2d","Example of TProfile2D usage", 200, 1., 5001., 200, 1., 5001., 1., 5001.);
      fProfile2D->SetDirectory(nullptr);
      AddObject(fProfile2D);
   }
}
//-----------------------------------------------------------
// event function
Bool_t TXXXProc::BuildEvent(TGo4EventElement *target)
{
   // called by framework from TXXXEvent to fill it

   auto XXXEvent = static_cast<TXXXEvent *>(target);

   TGo4MbsEvent *source = (TGo4MbsEvent *) GetInputEvent();
   if(!source) {
      TGo4Log::Error("TXXXProc: no input event!");
      return kFALSE;
   }
   if(source->GetTrigger() > 11) {
      TGo4Log::Info("TXXXProc: Skip trigger event");
      XXXEvent->SetValid(kFALSE); // not store
      return kFALSE;
   }
   // first we fill the TXXXEvent with data from MBS source
   // we have up to two subevents, crate 1 and 2
   // Note that one has to loop over all subevents and select them by
   // crate number:   psubevt->GetSubcrate(),
   // procid:         psubevt->GetProcid(),
   // and/or control: psubevt->GetControl()
   // here we use only crate number

   source->ResetIterator();
   while(auto psubevt = source->NextSubEvent()) { // loop over subevents
      Int_t *pdata = psubevt->GetDataField();
      Int_t lwords = psubevt->GetIntLen();
      if(lwords > 8) lwords=8; // take only first 8 lwords

      if(psubevt->GetSubcrate() == 1) {
         for(Int_t i = 0; i<lwords; i++) {
            // Int_t index =  *pdata&0xfff;      // in case low word is index
            //Float_t value = (*pdata>>16)&0xfff; // and high word is data
            Float_t value = (Float_t)*pdata++;      // otherwise longword data
            Int_t index = i;                      // and index in order
            XXXEvent->fCrate1[index] = value; // copy to output event
         }
      }
      if(psubevt->GetSubcrate() == 2)
         for(Int_t i = 0; i<lwords; i++)
            XXXEvent->fCrate2[i] = (Float_t)*pdata++;
   }

   // now we fill histograms from XXXEvent
   if(fControl->fill) {
      for(Int_t i = 0; i<8; i++) {
         fCr1Ch[i]->Fill(XXXEvent->fCrate1[i]);
         fCr2Ch[i]->Fill(XXXEvent->fCrate2[i]);
      }
      Float_t value1 = XXXEvent->fCrate1[0];
      Float_t value2 = XXXEvent->fCrate1[1];
      Float_t value3 = XXXEvent->fCrate1[3];
      fHis1->Fill(value1); //fill histograms without gate
      fHis2->Fill(value2);

      fProfile->Fill(value1, value2, 1);
      fProfile2D->Fill(value1, value2, value3, 1);

      if (fconHis1->Test(value1)) fHis1gate->Fill(value1); //fill histograms with gate
      if (fconHis2->Test(value2)) fHis2gate->Fill(value2);
      // fill Cr1Ch1x2 for three polygons:
      if (fPolyCon->Test(value1,value2))       fCr1Ch1x2->Fill(value1,value2);
      if(((*fConArr)[0])->Test(value1,value2)) fCr1Ch1x2->Fill(value1,value2);
      if(((*fConArr)[1])->Test(value1,value2)) fCr1Ch1x2->Fill(value1,value2);
   }
   XXXEvent->SetValid(kTRUE); // to store
   return kTRUE;
}
