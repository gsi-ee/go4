#include "TXXXProc.h"

#include "Riostream.h"

#include "TH1.h"
#include "TH2.h"
#include "TCutG.h"
#include "snprintf.h"

#include "TGo4MbsEvent.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4CondArray.h"
#include "TGo4Picture.h"

#include "TXXXParam.h"

//***********************************************************
TXXXProc::TXXXProc() : TGo4EventProcessor("Proc")
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

   // Creation of parameters (check if restored from auto save file):
   if(GetParameter("Par1")==0) {
      // no auto save, create new
      fParam = new TXXXParam("Par1");
      AddParameter(fParam);
      cout << "**** TXXXProc: Created parameter" << endl;
   } else {
      // got them from autosave file, restore pointers
      fParam=(TXXXParam *)GetParameter("Par1");
      cout << "**** TXXXProc: Restored parameter from autosave" << endl;
   }

   for(int i=0;i<8;i++) {
      fCr1Ch[i] = MakeH1('I', Form("Crate1/Cr1Ch%02d",i+1), Form("Crate 1 channel %2d",i+1), 5000, 0., 5000.);
      fCr2Ch[i] = MakeH1('I', Form("Crate2/Cr2Ch%02d",i+1), Form("Crate 2 channel %2d",i+1), 5000, 0., 5000.);
   }

   fCr1Ch1x2 = MakeH2('I', "Cr1Ch1x2","Crate 1 channel 1x2", 200, 0., 5000., 200, 0., 5000.);
   fHis1 = MakeH1('I', "His1","Condition histogram", 5000, 0., 5000.);
   fHis2 = MakeH1('I', "His2","Condition histogram", 5000, 0., 5000.);
   fHis1gate = MakeH1('I', "His1g","Gated histogram", 5000, 0., 5000.);
   fHis2gate = MakeH1('I', "His2g","Gated histogram", 5000, 0., 5000.);

   fconHis1= MakeWinCond("cHis1", 100,2000, "His1");
   fconHis2= MakeWinCond("cHis2", 100,2000, "His2");

   Double_t cutpnts[3][2] = { {400, 800}, {700, 900}, {600, 1100} };
   fPolyCon = MakePolyCond("polycon", 3, cutpnts);

   fConArr   = (TGo4CondArray*)GetAnalysisCondition("polyconar");
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

   fcondSet = GetPicture("condSet");
   if (fcondSet==0) {
      // in the upper two pads, the condition limits can be set,
      // in the lower two pads, the resulting histograms are shown
      fcondSet = new TGo4Picture("condSet","Set conditions");
      fcondSet->SetLinesDivision(2,2,2);
      fcondSet->LPic(0,0)->AddObject(fHis1);
      fcondSet->LPic(0,1)->AddObject(fHis2);
      fcondSet->LPic(0,0)->AddCondition(fconHis1);
      fcondSet->LPic(0,1)->AddCondition(fconHis2);
      fcondSet->LPic(1,0)->AddObject(fHis1gate);
      fcondSet->LPic(1,1)->AddObject(fHis2gate);
      fcondSet->LPic(1,0)->SetFillAtt(4, 1001); // solid
      fcondSet->LPic(1,0)->SetLineAtt(4,1,1);
      fcondSet->LPic(1,1)->SetFillAtt(9, 1001); // solid
      fcondSet->LPic(1,1)->SetLineAtt(9,1,1);
      AddPicture(fcondSet);
      cout << "**** TXXXProc: Created condSet" << endl;
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
      cout << "**** TXXXProc: Created Picture" << endl;
   }
}

//-----------------------------------------------------------
// event function
Bool_t TXXXProc::BuildEvent(TGo4EventElement* target)
{  // called by framework. We dont fill any output event here at all

   TGo4MbsSubEvent* psubevt;
   Int_t index=0;
   Float_t value1=0;
   Float_t value2=0;
   Float_t value=0;
   Int_t lwords,i;
   Int_t *pdata;

   fInput = (TGo4MbsEvent* ) GetInputEvent();
   if(fInput == 0)
   {
      cout << "AnlProc: no input event !"<< endl;
      return kFALSE;
   }
   if(fInput->GetTrigger() > 11)
   {
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

   fInput->ResetIterator();
   while((psubevt = fInput->NextSubEvent()) != 0) // loop over subevents
   {
      pdata=psubevt->GetDataField();
      lwords= psubevt->GetIntLen();
      if(lwords > 8) lwords=8; // take only first 8 lwords

      if(psubevt->GetSubcrate() == 1)
      {
         for(i = 0; i<lwords; i++)
         {
            //index =  *pdata&0xfff;      // in case low word is index
            //value = (*pdata>>16)&0xfff; // and high word is data
            value=(Float_t)*pdata++;      // otherwise longword data
            index=i;                      // and index in order
            fCrate1[index] = value; // copy to output event
         }
      }
      if(psubevt->GetSubcrate() == 2)
      {
         for(i = 0; i<lwords; i++) fCrate2[i] = (Float_t)*pdata++;
      }
   }

   // now we fill histograms from the arrays
   for(i = 0; i<8; i++)
   {
      fCr1Ch[i]->Fill(fCrate1[i]);
      fCr2Ch[i]->Fill(fCrate2[i]);
   }
   value1=fCrate1[0];
   value2=fCrate1[1];
   fHis1->Fill(value1); //fill histograms without gate
   fHis2->Fill(value2);
   if(fconHis1->Test(value1))fHis1gate->Fill(value1); //fill histograms with gate
   if(fconHis2->Test(value2))fHis2gate->Fill(value2);
   // fill Cr1Ch1x2 for three polygons:
   if(fPolyCon->Test(value1,value2))        fCr1Ch1x2->Fill(value1,value2);
   if(((*fConArr)[0])->Test(value1,value2)) fCr1Ch1x2->Fill(value1,value2);
   if(((*fConArr)[1])->Test(value1,value2)) fCr1Ch1x2->Fill(value1,value2);
   return kTRUE;
}
