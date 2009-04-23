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

  anl=(TascaAnalysis *)TGo4Analysis::Instance();
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
// The anl function gets the histogram or creates it
  for(i =0;i<96;i++)
  {
	snprintf(chis,15,"Adc_%02d",i);
	if(i > 63)      snprintf(chead,63,"Mod 11 chan %2d",i-64);
	else if(i > 31) snprintf(chead,63,"Mod 09 chan %2d",i-32);
	else            snprintf(chead,63,"Mod 07 chan %2d",i);
	fAdc[i] = anl->CreateTH1I("Raw/AllAdc",chis,chead,5000,0.5,5000.5);
  }
  for(i =0;i<8;i++)
  {
		snprintf(chis,15,"GammaE_%d",i);
		snprintf(chead,63,"Gamma E raw %d",i);
		fGammaE[i] = anl->CreateTH1I ("Raw/Gamma",chis,chead,9000,0.5,9000.5);
		snprintf(chis,15,"GammaT_%d",i);
		snprintf(chead,63,"Gamma T raw %d",i);
		fGammaT[i] = anl->CreateTH1I ("Raw/Gamma",chis,chead,5000,0.5,5000.5);
  }
	fPedestal  = anl->CreateTH1I ("Raw","Pedestals","Pedestals",96,-0.5,95.5);
	fContent   = anl->CreateTH1I ("Raw","Contents","Contents",96,-0.5,95.5);
	fTree      = anl->CreateTH1I (0,"Tree","Leaf",5000,0.5,5000.5);
	fAdcAllRaw = anl->CreateTH1I ("Raw","AdcAllRaw","All adc raw",5000,0.5,5000.5);
	fAdcAllCal = anl->CreateTH1I ("Raw","AdcAllCal","All adc cal",5000,0.5,5000.5);

// pictures rows, columns
    Geraw = anl->CreatePicture("Raw","Gamma","Gamma raw",8,2);
    M1raw = anl->CreatePicture("Raw","V785_1","Module 7",8,4);
    M2raw = anl->CreatePicture("Raw","V785_2","Module 9",8,4);
    M3raw = anl->CreatePicture("Raw","V785_3","Module 11",8,4);
  Int_t m=0;
  // enlarge stats box and position in [0:1] coordinates
  // show only Mean value (ROOT manual "Statistics Display")
  for(i=0;i<8;i++){ // 8 rows
	  anl->SetPicture(Geraw,fGammaE[i],i,0,1);
	  anl->SetPicture(Geraw,fGammaT[i],i,1,1);
	  for(k=0;k<4;k++){ // 4 columns
		  anl->SetPicture(M1raw,fAdc[m],i,k,1);
		  anl->SetPicture(M2raw,fAdc[m+32],i,k,1);
		  anl->SetPicture(M3raw,fAdc[m+64],i,k,1);
	  m++;
  }}
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
