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

  Text_t chis[16];
  Text_t chead[64];
  Int_t i;

  //// init user analysis objects:

  // Creation of parameters (check if restored from auto save file):
  if(GetParameter("Par1")==0)
    {// no auto save, create new
      fParam = new TXXXParam("Par1");
      AddParameter(fParam);
      cout << "**** TXXXProc: Created parameter" << endl;
    }
  else // got them from autosave file, restore pointers
    {
      fParam=(TXXXParam *)GetParameter("Par1");
      cout << "**** TXXXProc: Restored parameter from autosave" << endl;
    }

  // Creation of histograms (check if restored from auto save file):
  if(GetHistogram("Crate1/Cr1Ch01")==0)
    {// no auto save, create new
      for(i =0;i<8;i++)
   {
     snprintf(chis,15,"Cr1Ch%02d",i+1);  snprintf(chead,63,"Crate 1 channel %2d",i+1);
          fCr1Ch[i] = new TH1I (chis,chead,5000,1,5000);
          AddHistogram(fCr1Ch[i],"Crate1");
     snprintf(chis,15,"Cr2Ch%02d",i+1);  snprintf(chead,63, "Crate 2 channel %2d",i+1);
          fCr2Ch[i] = new TH1I (chis,chead,5000,1,5000);
          AddHistogram(fCr2Ch[i],"Crate2");
   }
      fCr1Ch1x2 = new TH2I("Cr1Ch1x2","Crate 1 channel 1x2",200,1,5000,200,1,5000);
      AddHistogram(fCr1Ch1x2);
      fHis1     = new TH1I ("His1","Condition histogram",5000,1,5000);
      AddHistogram(fHis1);
      fHis2     = new TH1I ("His2","Condition histogram",5000,1,5000);
      AddHistogram(fHis2);
      fHis1gate = new TH1I ("His1g","Gated histogram",5000,1,5000);
      AddHistogram(fHis1gate);
      fHis2gate = new TH1I ("His2g","Gated histogram",5000,1,5000);
      AddHistogram(fHis2gate);
      cout << "**** TXXXProc: Created histograms" << endl;
    }
  else // got them from autosave file, restore pointers
    {
      for(i =0;i<8;i++)
   {
     snprintf(chis,15,"Crate1/Cr1Ch%02d",i+1); fCr1Ch[i] = (TH1I*)GetHistogram(chis);
     snprintf(chis,15,"Crate2/Cr2Ch%02d",i+1); fCr2Ch[i] = (TH1I*)GetHistogram(chis);
   }
      fCr1Ch1x2 = (TH2I*)GetHistogram("Cr1Ch1x2");
      fHis1     = (TH1I*)GetHistogram("His1");
      fHis2     = (TH1I*)GetHistogram("His2");
      fHis1gate = (TH1I*)GetHistogram("His1g");
      fHis2gate = (TH1I*)GetHistogram("His2g");
      cout << "**** TXXXProc: Restored histograms from autosave" << endl;
    }
  // Creation of conditions (check if restored from auto save file):
  if(GetAnalysisCondition("cHis1")==0)
    {// no auto save, create new
      fconHis1= new TGo4WinCond("cHis1");
      fconHis2= new TGo4WinCond("cHis2");
      fconHis1->SetValues(100,2000);
      fconHis2->SetValues(100,2000);
      AddAnalysisCondition(fconHis1);
      AddAnalysisCondition(fconHis2);

      Double_t xvalues[4]={400,700,600,400};
      Double_t yvalues[4]={800,900,1100,800};
      TCutG* mycut= new TCutG("cut1",4,xvalues,yvalues);
      fPolyCon= new TGo4PolyCond("polycon");
      fPolyCon->SetValues(mycut); // copies mycat into fPolyCon
      fPolyCon->Disable(true);   // means: condition check always returns true
      delete mycut; // mycat has been copied into the conditions
      AddAnalysisCondition(fPolyCon);

      xvalues[0]=1000;xvalues[1]=2000;xvalues[2]=1500;xvalues[3]=1000;
      yvalues[0]=1000;yvalues[1]=1000;yvalues[2]=3000;yvalues[3]=1000;
      mycut= new TCutG("cut2",4,xvalues,yvalues);
      fConArr = new TGo4CondArray("polyconar",4,"TGo4PolyCond");
      fConArr->SetValues(mycut);
      fConArr->Disable(true);   // means: condition check always returns true
      delete mycut; // mycat has been copied into the conditions
      AddAnalysisCondition(fConArr);
      cout << "**** TXXXProc: Created conditions" << endl;
    }
  else // got them from autosave file, restore pointers
    {
      fPolyCon  = (TGo4PolyCond*) GetAnalysisCondition("polycon");
      fConArr   = (TGo4CondArray*)GetAnalysisCondition("polyconar");
      fconHis1  = (TGo4WinCond*)  GetAnalysisCondition("cHis1");
      fconHis2  = (TGo4WinCond*)  GetAnalysisCondition("cHis2");
      fconHis1->ResetCounts();
      fconHis2->ResetCounts();
      fPolyCon->ResetCounts();
      fConArr->ResetCounts();
      cout << "**** TXXXProc: Restored conditions from autosave" << endl;
    }
  // connect histograms to conditions. will be drawn when condition is edited.
  fconHis1->SetHistogram("His1");
  fconHis2->SetHistogram("His2");
  fconHis1->Enable();
  fconHis2->Enable();
  fPolyCon->Enable();
  ((*fConArr)[0])->Enable();
  ((*fConArr)[1])->Enable(); // 2 and 3 remain disabled

  if (GetPicture("Picture")==0)
    {// no auto save, create new
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
      cout << "**** TXXXProc: Created pictures" << endl;
    }
  else  // got them from autosave file, restore pointers
    {
      fPicture = GetPicture("Picture");
      fcondSet = GetPicture("condSet");
      cout << "**** TXXXProc: Restored pictures from autosave" << endl;
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
