#include "TascaUnpackProc.h"
#include "TGo4EventEndException.h"

#include "Riostream.h"
#include <time.h>

#include "TObjArray.h"
#include "TObjString.h"
#include "TH1.h"
#include "TH2.h"
#include "TCutG.h"
#include "snprintf.h"
#include "TPaveStats.h"
#include "s_filhe_swap.h"
#include "s_bufhe_swap.h"

#include "TGo4MbsEvent.h"
#include "TGo4WinCond.h"
#include "TGo4PolyCond.h"
#include "TGo4CondArray.h"
#include "TGo4Picture.h"

#include "TascaParameter.h"
#include "TascaPedestals.h"
#include "TascaUnpackEvent.h"
#include "TascaAnalysis.h"

//***********************************************************
TascaUnpackProc::TascaUnpackProc() :
  TGo4EventProcessor()
{}
//***********************************************************
// this one is used
TascaUnpackProc::TascaUnpackProc(const char* name) :
   TGo4EventProcessor(name)
{
  cout << "Tasca> TascaUnpackProc: Create" << endl;

  Text_t chis[32];
  Text_t chead[64];

  fPedestals   = (TascaPedestals *) GetParameter("Pedestals");
  if(fPedestals==0){
	  fPedestals = new TascaPedestals("Pedestals");
	  AddParameter(fPedestals);
  }
  codec = (TascaCodec *) GetParameter("Codec");
  if(codec==0){
    codec = new TascaCodec("Codec");
    AddParameter(codec);
  }
  codec->setMap(false);
  evcount=0;

  // Creation of histograms:
  for(i =0;i<96;i++)
  {
	snprintf(chis,15,"Adc%02d",i);
	if(i > 63)      snprintf(chead,63,"Mod 11 chan %2d",i-64);
	else if(i > 31) snprintf(chead,63,"Mod 09 chan %2d",i-32);
	else            snprintf(chead,63,"Mod 07 chan %2d",i);
	fAdc[i] = anl->CreateTH1I("AllAdc",chis,chead,5000,0.5,5000.5);
  }
  for(i =0;i<8;i++)
  {
		snprintf(chis,15,"GammaE%d",i);
		snprintf(chead,63,"Gamma E raw %d",i);
		fGammaE[i] = anl->CreateTH1I ("GammaRaw",chis,chead,5000,0.5,5000.5);
		snprintf(chis,15,"GammaT%d",i);
		snprintf(chead,63,"Gamma T raw %d",i);
		fGammaT[i] = anl->CreateTH1I ("GammaRaw",chis,chead,5000,0.5,5000.5);
  }
	fPedestal  = anl->CreateTH1I (0,"Pedestals","Pedestals",96,-0.5,95.5);
	fContent   = anl->CreateTH1I (0,"Contents","Contents",96,-0.5,95.5);
	fTree      = anl->CreateTH1I (0,"Tree","Leaf",5000,0.5,5000.5);
	fAdcAllRaw = anl->CreateTH1I (0,"AdcAllRaw","All adc raw",5000,0.5,5000.5);
	fAdcAllCal = anl->CreateTH1I (0,"AdcAllCal","All adc cal",5000,0.5,5000.5);

// pictures
  if (GetPicture("M1raw")==0)
    {
      Geraw = new TGo4Picture("Geraw","Gamma raw",8,2);
      M1raw = new TGo4Picture("M1raw","Module 8",8,4);
      M2raw = new TGo4Picture("M2raw","Module 10",8,4);
      M3raw = new TGo4Picture("M3raw","Module 12",8,4);
      AddPicture(Geraw);
      AddPicture(M1raw);
      AddPicture(M2raw);
      AddPicture(M3raw);
      Int_t m=0;
      // 4 columns, 8 rows
      // enlarge stats box and position in [0:1] coordinates
      // show only Mean value (ROOT manual "Statistics Display")
      for(i=0;i<8;i++){
    	  Geraw->Pic(i,0)->AddH1(fGammaE[i]);
          Geraw->Pic(i,0)->SetStatsAttr(0.1,0.6,0.4,0.9,101); // mean and name
          Geraw->Pic(i,0)->SetAxisLabelFontSize(0, 0.07, 0); // Go4 v4.2
          Geraw->Pic(i,0)->SetAxisLabelFontSize(1, 0.07, 0); // Go4 v4.2
          Geraw->Pic(i,0)->SetHisTitle(false);
          Geraw->Pic(i,0)->SetTitleAttr(0.1,0.75,0.7,0.9);

    	  Geraw->Pic(i,1)->AddH1(fGammaT[i]);
          Geraw->Pic(i,1)->SetStatsAttr(0.1,0.6,0.4,0.9,101); // mean and name
          Geraw->Pic(i,1)->SetAxisLabelFontSize(0, 0.07, 0); // Go4 v4.2
          Geraw->Pic(i,1)->SetAxisLabelFontSize(1, 0.07, 0); // Go4 v4.2
          Geraw->Pic(i,1)->SetHisTitle(false);
          Geraw->Pic(i,1)->SetTitleAttr(0.1,0.75,0.7,0.9);

    	  for(k=0;k<4;k++){
    	  M1raw->Pic(i,k)->AddH1(fAdc[m]);
          M1raw->Pic(i,k)->SetStatsAttr(0.1,0.6,0.4,0.9,101); // mean and name
          M1raw->Pic(i,k)->SetAxisLabelFontSize(0, 0.07, 0); // Go4 v4.2
          M1raw->Pic(i,k)->SetAxisLabelFontSize(1, 0.07, 0); // Go4 v4.2
          M1raw->Pic(i,k)->SetHisTitle(false);
          M1raw->Pic(i,k)->SetTitleAttr(0.1,0.75,0.7,0.9);

    	  M2raw->Pic(i,k)->AddH1(fAdc[m+32]);
          M2raw->Pic(i,k)->SetStatsAttr(0.1,0.6,0.4,0.9,101);
          M2raw->Pic(i,k)->SetAxisLabelFontSize(0, 0.07, 0);
          M2raw->Pic(i,k)->SetAxisLabelFontSize(1, 0.07, 0);
          M2raw->Pic(i,k)->SetHisTitle(false);
          M2raw->Pic(i,k)->SetTitleAttr(0.1,0.75,0.7,0.9);

    	  M3raw->Pic(i,k)->AddH1(fAdc[m+64]);
          M3raw->Pic(i,k)->SetStatsAttr(0.1,0.6,0.4,0.9,101);
          M3raw->Pic(i,k)->SetAxisLabelFontSize(0, 0.07, 0);
          M3raw->Pic(i,k)->SetAxisLabelFontSize(1, 0.07, 0);
          M3raw->Pic(i,k)->SetHisTitle(false);
          M3raw->Pic(i,k)->SetTitleAttr(0.1,0.75,0.7,0.9);
    	  m++;
      }}
    }
  else
    {
      Geraw = GetPicture("Geraw");
      M1raw = GetPicture("M1raw");
      M2raw = GetPicture("M2raw");
      M3raw = GetPicture("M3raw");
      cout << "Tasca> TascaUnpackProc: Restored pictures from autosave" << endl;
    }
}
//***********************************************************
TascaUnpackProc::~TascaUnpackProc()
{
  cout << "Tasca> TascaUnpackProc: Delete" << endl;
}
//***********************************************************
void TascaUnpackProc::CalcPedestals(){
for(i =0;i<96;i++){
	fPedestal->SetBinContent(i,fAdc[i]->GetMean());
	fPedestals->SetPedestals(i,fAdc[i]->GetMean());
}}
//***********************************************************

//-----------------------------------------------------------
void TascaUnpackProc::TascaUnpack(TascaUnpackEvent* pUnpackEvent)
{
  TGo4MbsSubEvent* psubevt;
  UInt_t crate, address, channels, header, off;
  UInt_t lwords;
  UInt_t *pdata;
  UInt_t latches=0;
  UInt_t adcs=0;
  UInt_t patt0,patt1,patt2,patt3;
  UInt_t lat,lat0,lat1,lat2,lat3;

  pUnpackEvent->SetValid(kFALSE); // not to store
  fInput    = (TGo4MbsEvent* ) GetInputEvent(); // from this
  if((fInput != 0) &
     (fInput->GetTrigger()!=14) &
     (fInput->GetTrigger()!=15) &
     (fInput->GetDlen() != 36)){
  //cout << "Event=" << fInput->GetCount() << endl;
  fInput->ResetIterator();
  psubevt = fInput->NextSubEvent();
  pdata=(UInt_t *)psubevt->GetDataField();
  lwords= psubevt->GetIntLen();
  // get number of latches and number of ADCs
  latches = *pdata & 0xFFFF;
  adcs = (*pdata++ & 0xFFFF0000) >> 16;
  patt0=*pdata++; // latch base + 0 : time stamp
  lat=*pdata++; //
  lat0=*pdata++; // latch 0

  patt1=*pdata++; // latch base + 1
  lat1=*pdata++; // latch 1
  patt2=*pdata++; // latch base + 2
  lat2=*pdata++; // latch 2
  patt3=*pdata++; // latch base + 3
  lat3=*pdata++; // latch 3

  pUnpackEvent->fiTimeStamp=patt0;
// Build Mpx table
  codec->setMpxIndex(lat0,lat1,lat2,lat3);
// copy the index table to event to store in tree
  memcpy(pUnpackEvent->fiMpxi,codec->getMpxIndex(),sizeof(pUnpackEvent->fiMpxi));
// first copy ADC values into low and high arrays
while(adcs > 0){
  adcs--;
  header=*pdata++;
  codec->setValue(header);
  if(codec->isHeader()){
	  crate=codec->getCrate();
	  channels=codec->getCnt();
	  address=codec->getAddress();
	  //cout << "    Crate=" << crate << " addr=" << address << " channels=" << channels << endl;
	  switch (address){
		  case  8: off= 0; break;
		  case 10: off=32; break;
		  case 12: off=64; break;
		  default: off= 0; break;
	  }
	  for(i=0;i<channels;i++){
		  codec->setValue(*pdata++); // channel should be even
		  pUnpackEvent->fiAdc[off+codec->getChan()]=codec->getAdc();
		  fAdc[off+codec->getChan()]->Fill(codec->getAdc());
		  fAdcAllRaw->Fill(codec->getAdc());
		  fContent->Fill(off+codec->getChan());
	  }
	  pdata++; // skip EOB
	  pUnpackEvent->SetValid(kTRUE); // to store
  } else if(codec->isEob()){
	  //cout << "    EOB " << endl;
  } else if(!codec->isValid()){
	  cout << "    No data " << endl;
  } else {
	  cout << "    No header found " << header << endl;
  }
  } // loop over ADCs

// now will follow 8 channels Gamma E and T
for(i=0;i<8;i++){
	  pUnpackEvent->fiGammaE[i]=i+1;
	  pUnpackEvent->fiGammaT[i]=i+11;
	  fGammaE[i]->Fill(pUnpackEvent->fiGammaE[i]);
	  fGammaT[i]->Fill(pUnpackEvent->fiGammaT[i]);
}

if(fPedestals->fbCalibrate)	for(i=0;i<96;i++){
	pUnpackEvent->fiAdc[i]=pUnpackEvent->fiAdc[i]+
	(UInt_t )(fPedestals->ffOffset-fPedestals->ffPedestals[i]);
	fAdcAllCal->Fill(pUnpackEvent->fiAdc[i]);
}

// now fill the detector arrays
// StopX
for(i=0;i<codec->getStopXnoAdc();i++){
	k=codec->getStopXAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	pUnpackEvent->fiStopXL[n]=pUnpackEvent->fiAdc[2*k];
	pUnpackEvent->fiStopXH[n]=pUnpackEvent->fiAdc[2*k+1];
}
// StopY
for(i=0;i<codec->getStopYnoAdc();i++){
	k=codec->getStopYAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	pUnpackEvent->fiStopYL[n]=pUnpackEvent->fiAdc[2*k];
	pUnpackEvent->fiStopYH[n]=pUnpackEvent->fiAdc[2*k+1];
}
// Back
for(i=0;i<codec->getBacknoAdc();i++){
	k=codec->getBackAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	pUnpackEvent->fiBackL[n]=pUnpackEvent->fiAdc[2*k];
	pUnpackEvent->fiBackH[n]=pUnpackEvent->fiAdc[2*k+1];
}
// Veto
for(i=0;i<codec->getVetonoAdc();i++){
	k=codec->getVetoAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	pUnpackEvent->fiVetoL[n]=pUnpackEvent->fiAdc[2*k];
	pUnpackEvent->fiVetoH[n]=pUnpackEvent->fiAdc[2*k+1];
}
} // check for valid event
evcount++;
if(!fPedestals->fbCalibrate){
	if(evcount >= 10000) {
	CalcPedestals();
	evcount=0;
	//throw TGo4EventEndException(this);
}}
}
