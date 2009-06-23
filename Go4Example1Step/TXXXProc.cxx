#include "TXXXProc.h"

#include "Riostream.h"

#include "TH1.h"
#include "TH2.h"
#include "TROOT.h"
#include "TCutG.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4CondArray.h"
#include "TGo4Picture.h"
#include "TGo4MbsEvent.h"
#include "snprintf.h"

#include "TXXXControl.h"
#include "TXXXEvent.h"

//***********************************************************
TXXXProc::TXXXProc() : TGo4EventProcessor()
{
   cout << "**** TXXXProc: Create instance " << endl;
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

   //// init user analysis objects:

   // Parameters are created in analysis, therefore just take them out
   fControl = (TXXXControl *) GetParameter("Control");

   // This example analysis allows for en-disabling the histogram filling.
   // Macro setfill.C creates macro histofill.C to set histogram fill status in parameter "Control".
   // Executed in Analysis terminal input line by .x setfill.C(n) with n=0,1.
   // This macro histofill.C, not the auto save file, will set status.
   gROOT->ProcessLine(".x histofill.C");
   fControl->PrintParameter(0,0);

   cout << "**** TXXXProc: Produce histograms" << endl;
   // Creation of histograms (or take them from autosave)
   for(int i=0;i<8;i++) {
      fCr1Ch[i] = MakeTH1('I', Form("Crate1/Cr1Ch%02d",i+1), Form("Crate 1 channel %2d",i+1), 5000, 0., 5000.);
      fCr2Ch[i] = MakeTH1('I', Form("Crate2/Cr2Ch%02d",i+1), Form("Crate 2 channel %2d",i+1), 5000, 0., 5000.);
   }

   fCr1Ch1x2 = MakeTH2('I', "Cr1Ch1x2","Crate 1 channel 1x2", 200, 0., 5000., 200, 0., 5000.);
   fHis1 = MakeTH1('I', "His1","Condition histogram", 5000, 0., 5000.);
   fHis2 = MakeTH1('I', "His2","Condition histogram", 5000, 0., 5000.);
   fHis1gate = MakeTH1('I', "His1g","Gated histogram", 5000, 0., 5000.);
   fHis2gate = MakeTH1('I', "His2g","Gated histogram", 5000, 0., 5000.);

   cout << "**** TXXXProc: Produce conditions" << endl;
   fconHis1 = MakeWinCond("cHis1", 100, 2000, "His1");
   fconHis2 = MakeWinCond("cHis2", 100, 2000, "His2");

   Double_t cutpnts[3][2] = { {400, 800}, {700, 900}, {600, 1100} };
   fPolyCon = MakePolyCond("polycon", 3, cutpnts);

   fConArr = (TGo4CondArray*)GetAnalysisCondition("polyconar");
   if(fConArr==0) {
      // This is example how to create condition array
      cout << "**** TXXXProc: Create condition" << endl;
      Double_t xvalues[4] = { 1000, 2000, 1500, 1000 };
      Double_t yvalues[4] = { 1000, 1000, 3000, 1000 };
      TCutG* mycut = new TCutG("cut2", 4, xvalues, yvalues);
      fConArr = new TGo4CondArray("polyconar",4,"TGo4PolyCond");
      fConArr->SetValues(mycut);
      fConArr->Disable(true);   // means: condition check always returns true
      delete mycut; // mycat has been copied into the conditions
      AddAnalysisCondition(fConArr);
   } else {
      cout << "**** TXXXProc: Restore condition from autosave" << endl;
      fConArr->ResetCounts();
   }
   // connect histograms to conditions. will be drawn when condition is edited.
   ((*fConArr)[0])->Enable();
   ((*fConArr)[1])->Enable(); // 2 and 3 remain disabled

   cout << "**** TXXXProc: Produce pictures" << endl;
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

   fPicture = GetPicture("Picture");
   if (fPicture == 0) {
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
}
//-----------------------------------------------------------
// event function
Bool_t TXXXProc::BuildEvent(TGo4EventElement* target)
{
   // called by framework from TXXXEvent to fill it

   TXXXEvent* XXXEvent = (TXXXEvent*) target;

   TGo4MbsEvent* source = (TGo4MbsEvent*) GetInputEvent();
   if(source == 0) {
      cout << "AnlProc: no input event !"<< endl;
      return kFALSE;
   }
   if(source->GetTrigger() > 11) {
      cout << "**** TXXXProc: Skip trigger event"<<endl;
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
   TGo4MbsSubEvent* psubevt(0);
   while((psubevt = source->NextSubEvent()) != 0) { // loop over subevents
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
      fHis1->Fill(value1); //fill histograms without gate
      fHis2->Fill(value2);
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
