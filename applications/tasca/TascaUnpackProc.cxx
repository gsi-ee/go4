//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaUnpackProc.h"
#include "TGo4EventEndException.h"

#include "Riostream.h"
#include <time.h>
#include <stdlib.h>

#include "TRandom.h"
#include <math.h>
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

#include "TascaControl.h"
#include "TascaParameter.h"
#include "TascaPedestals.h"
#include "TascaUnpackEvent.h"
#include "TascaAnalysis.h"

#define V785_1 7
#define V785_2 8
#define V785_3 9

//***********************************************************
TascaUnpackProc::~TascaUnpackProc()
{
  cout << "Tasca> TascaUnpackProc: Processed "<<fiEventsProcessed<<
  " written "<<fiEventsWritten <<
  " last "<<fLastEvent<<endl;
}
//***********************************************************
TascaUnpackProc::TascaUnpackProc() :
  TGo4EventProcessor()
{}
//***********************************************************
// this one is used
TascaUnpackProc::TascaUnpackProc(const char* name) :
   TGo4EventProcessor(name)
{
  cout << "Tasca> TascaUnpackProc "<<name<<" created" << endl;

  Text_t chis[32];
  Text_t chead[64];
  spillTime0=0;
  spillIndex=0;
  spillOn=kFALSE;
  spillTest=kFALSE;
  memset((void*) spillBins,0, sizeof(spillBins));

  anl=(TascaAnalysis *)TGo4Analysis::Instance();

  fParam     = (TascaParameter *) anl->CreateParameter("Parameter","Parameters");
  fControl   = (TascaControl *)   anl->CreateParameter("Control","Controls");
  fPedestals = (TascaPedestals *) anl->CreateParameter("Pedestals","Pedestals");
  codec      = (TascaCodec *)     anl->CreateParameter("Codec","Codec");
  // reset counters
  gROOT->ProcessLine(".x setcontrol.C()");
  gROOT->ProcessLine(".x setparam.C()");
  fControl->TofChecked=0;
  fControl->TofTrue=0;
  fControl->ChopperChecked=0;
  fControl->ChopperTrue=0;
  fControl->MicroChecked=0;
  fControl->MicroTrue=0;
  fControl->MacroChecked=0;
  fControl->MacroTrue=0;
  cout << "Tasca> TascaUnpackProc: Reset check counters" << endl;
  codec->Cleanup();
  codec->setMap(false); // set true to get printout
  fiEventsProcessed=0;
  fiEventsWritten=0;
  TimeLastgamma=0;
  TimeLastsec=0;
  TimeLastmysec=0;
  TimeLastadc=0;
  fLastEvent=0;
  lastfilenum=0;

// Creation of histograms:
// The anl function gets the histogram or creates it
  for(i=0;i<codec->SCHANNELS;i++)
  {
      snprintf(chis,15,"TraceRaw_%02d",i);
      snprintf(chead,63,"Raw Data channel %2d",i);
      fTrace[i] = anl->CreateTH1I ("Unpack/SIS3302/TraceR",chis,chead,codec->SMAX_RAW,0.5,codec->SMAX_RAW+0.5);

      snprintf(chis,15,"TraceE_%02d",i);
      snprintf(chead,63,"E Filter channel %2d",i);
      fTrace_e[i] = anl->CreateTH1I ("Unpack/SIS3302/TraceE",chis,chead,codec->SMAX_E,0.5,codec->SMAX_E+0.5);

      snprintf(chis,15,"Histo_%02d",i);
      snprintf(chead,63,"Histo channel %2d",i);
      fHisto[i] = anl->CreateTH1I ("Unpack/SIS3302/Histo",chis,chead,8192,0.5,8191.5);

      snprintf(chis,15,"Pileup_%02d",i);
      snprintf(chead,63,"Pileup channel %2d",i);
      fPileup[i] = anl->CreateTH1I ("Unpack/SIS3302/Pileup",chis,chead,1024,0.5,1023.5);
  }
  if(fControl->UnpackHisto){
  for(i =0;i<96;i++)
  {
	snprintf(chis,15,"Adc_%02d",i);
	if(i > 63)      snprintf(chead,63,"Mod 3 chan %2d",i-64);
	else if(i > 31) snprintf(chead,63,"Mod 2 chan %2d",i-32);
	else            snprintf(chead,63,"Mod 1 chan %2d",i);
	fAdc[i] = anl->CreateTH1I("Unpack/AllAdc",chis,chead,5000,0.5,5000.5);
  }
  }
  for(i =0;i<7;i++)
  {
		snprintf(chis,15,"GammaE_%d",i);
		snprintf(chead,63,"Gamma E raw %d",i);
		fGammaE[i] = anl->CreateTH1I ("Unpack/GammaE",chis,chead,9000,0.5,9000.5);
		snprintf(chis,15,"GammaT_%d",i);
		snprintf(chead,63,"Gamma T raw %d",i);
		fGammaT[i] = anl->CreateTH1I ("Unpack/GammaT",chis,chead,"[10ns]","Counts",1000,0.5,1000.5);
  }
  fGammaTime  = anl->CreateTH1I ("Unpack","GammaTimeDelta","Gamma delta time","[mysec]","Events",5000,0.5,20000.5);
  fSystemTime = anl->CreateTH1I ("Unpack","SystemTimeDelta","System delta time","[mysec]","Events",5000,0.5,20000.5);
  fAdcTime    = anl->CreateTH1I ("Unpack","AdcTimeDelta","Adc  delta time","[mysec]","Events",5000,0.5,20000.5);
  fMultiGamma = anl->CreateTH1I ("Unpack/Multi","MultiGamma","Gamma multiplicity",10,0,10);
  fMultiAdc   = anl->CreateTH1I ("Unpack/Multi","MultiAdc","Adc multiplicity",100,0,100);
  fMultiStopXL = anl->CreateTH1I ("Unpack/Multi","MultiStopXL","Stop XL multiplicity",144,0,144);
  fMultiStopXH = anl->CreateTH1I ("Unpack/Multi","MultiStopXH","Stop XH multiplicity",144,0,144);
  fMultiStopYL = anl->CreateTH1I ("Unpack/Multi","MultiStopYL","Stop YL multiplicity",144,0,144);
  fMultiStopYH = anl->CreateTH1I ("Unpack/Multi","MultiStopYH","Stop YH multiplicity",144,0,144);
  fMultiBackL = anl->CreateTH1I ("Unpack/Multi","MultiBackL","Back L multiplicity",64,0,64);
  fMultiBackH = anl->CreateTH1I ("Unpack/Multi","MultiBackH","Back H multiplicity",64,0,64);
  fMultiVetoL = anl->CreateTH1I ("Unpack/Multi","MultiVetoL","Veto L multiplicity",16,0,16);
  fMultiVetoH = anl->CreateTH1I ("Unpack/Multi","MultiVetoH","Veto H multiplicity",16,0,16);
  fTof        = anl->CreateTH1I ("Unpack","Tof","Adc[80]",1000,0,4000);
  fTofgated   = anl->CreateTH1I ("Unpack","Tofg","Adc[80] gated with TOF",1000,0,4000);
  // test spectrum
//	fTest      = anl->CreateTH1I (0,"Gaussians","Gauss",4000,0.5,4000.5);
//	  gRandom->SetSeed(0);
//	  for(i=0;i<4000;i++){
//		  Double_t v1,v2,s;
//		  do{
//    	v1 = 2.*gRandom->Rndm()-1.;
//    	v2 = 2.*gRandom->Rndm()-1.;
//    	s = v1*v1+v2*v2;
//      } while (s >= 1.0 || s == 0.0);
//	fTest->Fill(30.*v1*(sqrt(-2.*log(s)/s)) + 500.);
//	fTest->Fill(40.*v2*(sqrt(-2.*log(s)/s)) + 2000.);
//	fTest->Fill(30.*v1*(sqrt(-2.*log(s)/s)) + 1500.);
//	fTest->Fill(40.*v2*(sqrt(-2.*log(s)/s)) + 2500.);
//	  }

	fSize      = anl->CreateTH1I ("Unpack","Size","Netto event size","Size [32bit]","Events",200,0,200);
	fSizeA     = anl->CreateTH1I ("Unpack","SizeA","Netto ADC event size","Size [32bit]","Events",200,0,200);
	fSizeG     = anl->CreateTH1I ("Unpack","SizeG","Netto GAMMA event size","Size [32bit]","Events",200,0,200);
	fSpill     = anl->CreateTH1I ("Unpack","Spill","Events over spill","Mysec","Events",10000,0,200000);
	fSpillG    = anl->CreateTH1I ("Unpack","SpillGated","Events over spill with stop E","Mysec","Events",10000,0,200000);
	fFilter    = anl->CreateTH1I ("Unpack","Filter","Tof,chopper,macro,micro","0:all, 1:checked, 2:true, 3:false, 5: 9: 13:","Counts",17,0,17);
	fPedestal  = anl->CreateTH1I ("Unpack","Pedestals","Pedestals",96,-0.5,95.5);
	fContent   = anl->CreateTH1I ("Unpack","Contents","Contents",96,-0.5,95.5);
	fTree      = anl->CreateTH1I (0,"Tree","Leaf",5000,0.5,5000.5);
	fAdcAllRaw = anl->CreateTH1I ("Unpack","AdcAllRaw","All adc raw",5000,0.5,5000.5);

// pictures rows, columns
    Multi = anl->CreatePicture("Unpack","Multi","Multiplicities",5,2);
    Geraw = anl->CreatePicture("Unpack","pGamma","Gamma raw",8,2);
    M1raw = anl->CreatePicture("Unpack","pV785_1","Module 7",8,4);
    M2raw = anl->CreatePicture("Unpack","pV785_2","Module 9",8,4);
    M3raw = anl->CreatePicture("Unpack","pV785_3","Module 11",8,4);
	anl->SetPicture(Multi,fMultiAdc,0,0,1);
	anl->SetPicture(Multi,fMultiStopXL,1,0,1);
	anl->SetPicture(Multi,fMultiStopXH,2,0,1);
	anl->SetPicture(Multi,fMultiStopYL,3,0,1);
	anl->SetPicture(Multi,fMultiStopYH,4,0,1);
	anl->SetPicture(Multi,fMultiGamma,0,1,1);
	anl->SetPicture(Multi,fMultiBackL,1,1,1);
	anl->SetPicture(Multi,fMultiBackH,2,1,1);
	anl->SetPicture(Multi,fMultiVetoL,3,1,1);
	anl->SetPicture(Multi,fMultiVetoH,4,1,1);
  Int_t m=0;
  // enlarge stats box and position in [0:1] coordinates
  // show only Mean value (ROOT manual "Statistics Display")
  for(i=0;i<7;i++){
	  anl->SetPicture(Geraw,fGammaE[i],i,0,1);
	  anl->SetPicture(Geraw,fGammaT[i],i,1,1);
  }
  anl->SetPicture(Geraw,fGammaTime,7,1,1);
  for(i=0;i<8;i++){ // 8 rows
	  if(fControl->UnpackHisto){
	  for(k=0;k<4;k++){ // 4 columns
		  anl->SetPicture(M1raw,fAdc[m],i,k,1);
		  anl->SetPicture(M2raw,fAdc[m+32],i,k,1);
		  anl->SetPicture(M3raw,fAdc[m+64],i,k,1);
		  m++;
	  }}
  }
}
//***********************************************************
void TascaUnpackProc::CalcPedestals(){
if(fControl->UnpackHisto){
for(i =0;i<96;i++){
	if(fPedestal)fPedestal->SetBinContent(i,fAdc[i]->GetMean());
	fPedestals->SetPedestals(i,fAdc[i]->GetMean());
}}}
//***********************************************************

//-----------------------------------------------------------
void TascaUnpackProc::TascaUnpack(TascaUnpackEvent* pUP)
{
  TGo4MbsSubEvent* psubevt;
  UInt_t H,L;
  low=fParam->AdcThreshold;
  takeEvent=kFALSE;
  latches=1;
  adcs=0;
  pUnpackEvent=pUP;
  pUnpackEvent->SetValid(kFALSE);
  fInput    = (TGo4MbsEvent* ) GetInputEvent(); // from this
  fSize->Fill((fInput->GetDlen()>>1)-5);// is data length in 32bit without headers
  if((fInput == 0) |
     (fInput->GetTrigger()==14) |
     (fInput->GetTrigger()==15)) {
	  return;
  }
  // file number
  strcpy(cfilename,fInput->GetEventSource()->GetActiveName());
  memcpy(&cfilenum,&cfilename[strlen(cfilename)-8],4);
  cfilenum[4]=0;
  pUnpackEvent->fiFileNumber=atoi(cfilenum);
  // run number
  memcpy(&cfilenum,&cfilename[strlen(cfilename)-11],3);
  cfilenum[3]=0;
  pUnpackEvent->fiFileNumber += (atoi(cfilenum)<<16);
  //cout<<endl<<"Unp: "<<fInput->GetCount()<<endl;
  fiEventsProcessed++;
  fInput->ResetIterator();
  psubevt = fInput->NextSubEvent();
  pdata=(UInt_t *)psubevt->GetDataField();
  lwords= psubevt->GetIntLen(); // data length 32 bit
  pbehind=pdata+lwords;
  // look for system time stamp
  if(*pdata == 0xaffeaffe){
	  pdata++;
	  pUnpackEvent->fiSystemSec=*pdata++;
	  pUnpackEvent->fiSystemMysec=*pdata++;
  }
  // get number of latches and number of V785 ADCs
  latches = *pdata & 0xFFFF;
  adcs    = (*pdata & 0xFFFF0000) >> 16;

  psubevent=pdata;
  // Get V785 data
  if(adcs > 0){
  pdata++;
  patt0=*pdata++; // abcd
  timestamp=*pdata++; // latch base : time stamp
  lat0=*pdata++; // latch 0

  patt1=*pdata++; // latch base + 1
  lat1=*pdata++; // latch 1
  patt2=*pdata++; // latch base + 2
  lat2=*pdata++; // latch 2
  patt3=*pdata++; // latch base + 3
  lat3=*pdata++; // latch 3
  codec->setMpxIndex(lat0,lat1,lat2,lat3);
// Spill event counts
  spillTest=codec->isMacro();
//  if(fInput->GetTrigger()==12) spillTest=kTRUE;
//  if(fInput->GetTrigger()==13) spillTest=kFALSE;
  if(spillTest){ // spill on
  if(!spillOn) spillTime0=timestamp; // was off, save time
  else{
	  if(timestamp<spillTime0) timediff=0xFFFFFFFF-spillTime0+timestamp+1;
	  else                     timediff=timestamp-spillTime0;
	  fSpill->Fill(timediff);
  }
  spillOn=kTRUE;
  }else spillOn=kFALSE; // spill off

//-----
  pUnpackEvent->fiTimeStamp=timestamp; // mysec
  pUnpackEvent->fiEventNumber=fInput->GetCount();
  if(fLastEvent+1 != pUnpackEvent->fiEventNumber){
	  cout <<"      Unpack:  Last event  "<<fLastEvent<<", this "<<pUnpackEvent->fiEventNumber<<endl;
	  cout <<"      Unpack:  First event "<<pUnpackEvent->fiEventNumber<<endl;
  }
  fLastEvent= pUnpackEvent->fiEventNumber;
 // cout <<"Unp: "<<pUnpackEvent->fiEventNumber<< endl;
  // register time
  if(timestamp<TimeLastadc) fiDeltaTime=0xFFFFFFFF-TimeLastadc+timestamp+1;
  else                      fiDeltaTime=timestamp-TimeLastadc;
  TimeLastadc=timestamp;
  // system time, sec minus offset
  msec=pUnpackEvent->fiSystemMysec/1000;
  if(TimeLastsec > 0)
    pUnpackEvent->fiSystemMysec += (pUnpackEvent->fiSystemSec-TimeLastsec)*1000000;
  TimeLastsec=pUnpackEvent->fiSystemSec;
  if(pUnpackEvent->fiSystemMysec<TimeLastmysec) fiDeltaSystemTime=0xFFFFFFFF-TimeLastmysec+pUnpackEvent->fiSystemMysec+1;
  else                                          fiDeltaSystemTime=pUnpackEvent->fiSystemMysec-TimeLastmysec;
  TimeLastmysec=pUnpackEvent->fiSystemMysec;
  pUnpackEvent->fiSystemSec = (pUnpackEvent->fiSystemSec-fParam->SystemTimeSecOff)*1000; //msec
  pUnpackEvent->fiSystemSec += msec;//msec
  // need first and last event in file to determine the time window of the file
  if(lastfilenum!=pUnpackEvent->fiFileNumber){ // file changed, this event is first in file
	lastfilenum=pUnpackEvent->fiFileNumber;
  }

  fSystemTime->Fill(fiDeltaSystemTime);
  fAdcTime->Fill(fiDeltaTime);
// Build Mpx table
// check conditions to select events
fFilter->Fill(0); // all events
pUnpackEvent->fisTof=codec->isTof();
pUnpackEvent->fisMacro=codec->isMacro();
pUnpackEvent->fisMicro=codec->isMicro();
pUnpackEvent->fisChopper=codec->isChopper();
  if(codec->isTof())	fFilter->Fill(2);
  else 					fFilter->Fill(3);
  if(codec->isChopper())fFilter->Fill(6);
  else 					fFilter->Fill(7);
  if(codec->isMacro())	fFilter->Fill(10);
  else 					fFilter->Fill(11);
  if(codec->isMicro())	fFilter->Fill(14);
  else 					fFilter->Fill(15);
  if(fControl->checkTof){
	  fControl->TofChecked++;
	  fFilter->Fill(1);
	  takeEvent=(fControl->TofMustbe==codec->isTof());
	  if(!takeEvent)return;
	  fControl->TofTrue++;
  }
  if(fControl->checkChopper){
	  fControl->ChopperChecked++;
	  fFilter->Fill(5);
	  takeEvent=(fControl->ChopperMustbe==codec->isChopper());
	  if(!takeEvent)return;
	  fControl->ChopperTrue++;
  }
  if(fControl->checkMacro){
	  fControl->MacroChecked++;
	  fFilter->Fill(9);
	  takeEvent=(fControl->MacroMustbe==codec->isMacro());
	  if(!takeEvent)return;
	  fControl->MacroTrue++;
  }
  if(fControl->checkMicro){
	  fControl->MicroChecked++;
	  fFilter->Fill(13);
	  takeEvent=(fControl->MicroMustbe==codec->isMicro());
	  if(!takeEvent)return;
	  fControl->MicroTrue++;
  }
  takeEvent=kFALSE;
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
	  switch (adcs){ // 2 is first, 0 last
		  case 2: off= 0; break;
		  case 1: off=32; break;
		  case 0: off=64; break;
		  default: off= 0; cout<<"Unknown V785 "<<address<<endl;break;
	  }
	  for(i=0;i<channels;i++){
		  codec->setValue(*pdata++);
		  pUnpackEvent->fiAdc[off+codec->getChan()]=codec->getAdc();
		  fAdcAllRaw->Fill(codec->getAdc());
		  fContent->Fill(off+codec->getChan());
		  pUnpackEvent->fiMultiAdc++;
	  }
	  pdata++; // skip EOB
	  takeEvent=kTRUE;
  } else if(codec->isEob()){
	  //cout << "    EOB " << endl;
  } else if(!codec->isValid()){
      //cout << "    No data " <<pUnpackEvent->fiEventNumber<< endl;
  } else {
	  //cout << "    No header found " << header << endl;
  }
  } // loop over ADCs
if(fControl->UnpackHisto){
for(i=0;i<96;i++) fAdc[i]->Fill(pUnpackEvent->fiAdc[i]);
}
fMultiAdc->Fill(pUnpackEvent->fiMultiAdc);
pUnpackEvent->SetValid(takeEvent); // to store
pUnpackEvent->fisTof=pUnpackEvent->fiAdc[80] > fParam->Adc80TofMin;
// now fill the detector arrays. Low is even, high is odd index in fiAdc
// StopX
multiL=0;
multiH=0;
for(i=0;i<codec->getStopXnoAdc();i++){
	k=codec->getStopXAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	//cout << "k " << k << " n " << n << " adc "<< H<<endl;
	L=pUnpackEvent->fiAdc[2*k];
	if(L>fParam->AdcThreshold){
		multiL++;
		if(L>pUnpackEvent->fiStopXLhitV){
			pUnpackEvent->fiStopXLhitV=L;
			pUnpackEvent->fiStopXLhitI=n;
		}
	}
	H=pUnpackEvent->fiAdc[2*k+1];
	if(H>fParam->AdcThreshold){
		multiH++;
		if(H>pUnpackEvent->fiStopXHhitV){
			pUnpackEvent->fiStopXHhitV=H;
			pUnpackEvent->fiStopXHhitI=n;
			//printf("n %d adc %d\n",pUnpackEvent->fiStopXHhitI,pUnpackEvent->fiStopXHhitV);
		}
	}
	pUnpackEvent->fiStopXL[n]=L;
	pUnpackEvent->fiStopXH[n]=H;
}//StopX
pUnpackEvent->fiMultiStopXL=multiL;
pUnpackEvent->fiMultiStopXH=multiH;
fMultiStopXL->Fill(multiL);
fMultiStopXH->Fill(multiH);
// StopY
multiL=0;
multiH=0;
for(i=0;i<codec->getStopYnoAdc();i++){
	k=codec->getStopYAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	L=pUnpackEvent->fiAdc[2*k];
	if(L>fParam->AdcThreshold){
		multiL++;
		if(L>pUnpackEvent->fiStopYLhitV){
			pUnpackEvent->fiStopYLhitV=L;
			pUnpackEvent->fiStopYLhitI=n;
		}
	}
	H=pUnpackEvent->fiAdc[2*k+1];
	if(H>fParam->AdcThreshold){
		multiH++;
		if(H>pUnpackEvent->fiStopYHhitV){
			pUnpackEvent->fiStopYHhitV=H;
			pUnpackEvent->fiStopYHhitI=n;
		}
	}
	pUnpackEvent->fiStopYL[n]=L;
	pUnpackEvent->fiStopYH[n]=H;
}//StopY
pUnpackEvent->fiMultiStopYL=multiL;
pUnpackEvent->fiMultiStopYH=multiH;
fMultiStopYL->Fill(multiL);
fMultiStopYH->Fill(multiH);

// Back
multiL=0;
multiH=0;
for(i=0;i<codec->getBacknoAdc();i++){
	k=codec->getBackAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	L=pUnpackEvent->fiAdc[2*k];
	if(L>0){
		multiL++;
		if(L>pUnpackEvent->fiBackLhitV){
			pUnpackEvent->fiBackLhitV=L;
			pUnpackEvent->fiBackLhitI=n;
		}
	}
	H=pUnpackEvent->fiAdc[2*k+1];
	if(H>0){
		multiH++;
		if(H>pUnpackEvent->fiBackHhitV){
			pUnpackEvent->fiBackHhitV=H;
			pUnpackEvent->fiBackHhitI=n;
		}
	}
	pUnpackEvent->fiBackL[n]=L;
	pUnpackEvent->fiBackH[n]=H;
}// Back
pUnpackEvent->fiMultiBackL=multiL;
pUnpackEvent->fiMultiBackH=multiH;
fMultiBackL->Fill(multiL);
fMultiBackH->Fill(multiH);
// Veto
multiL=0;
multiH=0;
for(i=0;i<codec->getVetonoAdc();i++){
	k=codec->getVetoAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	L=pUnpackEvent->fiAdc[2*k];
	if(L>0){
		multiL++;
		if(L>pUnpackEvent->fiVetoLhitV){
			pUnpackEvent->fiVetoLhitV=L;
			pUnpackEvent->fiVetoLhitI=n;
		}
	}
	H=pUnpackEvent->fiAdc[2*k+1];
	if(H>0){
		multiH++;
		if(H>pUnpackEvent->fiVetoHhitV){
			pUnpackEvent->fiVetoHhitV=H;
			pUnpackEvent->fiVetoHhitI=n;
		}
	}
	pUnpackEvent->fiVetoL[n]=L;
	pUnpackEvent->fiVetoH[n]=H;
}//Veto
pUnpackEvent->fisVeto=(multiL>0);
pUnpackEvent->fiMultiVetoL=multiL;
pUnpackEvent->fiMultiVetoH=multiH;
fMultiVetoL->Fill(multiL);
fMultiVetoH->Fill(multiH);
}// V785 ADCs

if(spillOn&(pUnpackEvent->fiStopXHhitV>fParam->AlphaMinH))fSpillG->Fill(timediff);
if(pdata-psubevent) fSizeA->Fill(pdata-psubevent);

// follows Sis3302
if(pdata != pbehind){
  pdata++; // skip first tag word
  DecodeGamma(pdata,pbehind);
  takeEvent=kTRUE;
  for(i=0;i<codec->SCHANNELS-1;i++){
	fGammaE[i]->Fill(pUnpackEvent->fiGammaE[i]);
	if(pUnpackEvent->fiGammaE[i]>0)
		pUnpackEvent->fiGammaChannelTime[i]=(Int_t)(fGammaStamps[i]-fGammaStamps[7]);
	fGammaT[i]->Fill(pUnpackEvent->fiGammaChannelTime[i]);
  }
  fMultiGamma->Fill(pUnpackEvent->fiMultiGamma);
  if(pUnpackEvent->fiGammaTime<TimeLastgamma) timediff=0xFFFFFFFF-TimeLastgamma+pUnpackEvent->fiGammaTime+1;
  else                      timediff=pUnpackEvent->fiGammaTime-TimeLastgamma;
  fiDeltaGammaTime=timediff;
  fGammaTime->Fill(fiDeltaGammaTime/100);
  TimeLastgamma=pUnpackEvent->fiGammaTime;
  if(pbehind-pdata)  fSizeG->Fill(pbehind-pdata);
}
pUnpackEvent->SetValid(takeEvent); // to store
if(takeEvent)fiEventsWritten++;
return;
 // check for valid event
//if(!fPedestals->Calibrate){
//	if(evcount >= 10000) {
//	CalcPedestals();
//	evcount=0;
//	}}
//throw TGo4EventEndException(this); // to stop analysis
}
Bool_t TascaUnpackProc::DecodeGamma(UInt_t* data, UInt_t* behind)
{  // called by framework. We dont fill any output event here at all


UInt_t *pl_data,
first_e_sample;

unsigned short *pl_data16,
first_sample;

long i,
samples,    // no of samples per trace
chan,
card,       // card number
energy=0,
trg_cnt,
ft_cnt,
trailer;

long buffer_length,  // private header info
raw_data,
energy_data;

static long raw_data_old=0;

char index_pat;

long base_sum,
base2_sum,
top_sum;


pl_data=data;
//*****************************************************************
// NEW 13-03-09, skip decimation and sample_frequency header word

pl_data++;

//*****************
// channel loop
//*****************

while(1)
{
	if(pl_data >= behind)return kFALSE;

	//******************************
	// 3 (private) header longwords
	//******************************

	buffer_length=(long)*pl_data++;
	raw_data=(long)*pl_data++;
	energy_data=(long)*pl_data++;

	//*****************************
	// NEW 13-03-09, skip peaking/gap time information

	pl_data++;

	// NEW 09.03.09 -------------------------------------------------------------------------
	// leave loop here before sorting of data because of online change in raw_data parameter
	// otherwise GO4 might crash - still don't know why

//	if (raw_data != raw_data_old && raw_data_old != 0)
//	{
//		raw_data_old=raw_data;
//		break;
//	}

	//------------------------------------------------------------------------------------

	//sleep(1);
	//printf("buffer_length: 0x%08x\n",(int)buffer_length);
	//printf("     raw_data: 0x%08x\n",(int)raw_data);
	//printf(" raw_data_old: 0x%08x\n",(int)raw_data_old);
	//printf("  energy_data: 0x%08x\n\n",(int)energy_data);
	//fflush(stdout);

	//*****************************
	//  beginning of struck data
	//*****************************

	card=(buffer_length&0xFF000000)>>24;
	chan=(buffer_length&0x00FF0000)>>16;

	fGammaStamps[chan]=((ULong64_t)*pl_data)<<32;  // high part
	pl_data++;
	fGammaStamps[chan]+=*pl_data;  // low part
	if(chan==7)	{
		pUnpackEvent->fiGammaTime=*pl_data;
		return kFALSE;	}
	pl_data++;
	//        pl_data16 = (INTS2 *) pl_data;

	//****************
	//   Raw Data
	//****************

	if (raw_data)
	{
		raw_data_old = raw_data;   // NEW 09.03.09

		pl_data16 = (unsigned short *) pl_data;
		samples=raw_data;

		if (raw_data>codec->SMAX_RAW)
		{
			printf("Raw Data too long!\n");
			break;
		}

		first_sample = *pl_data16;   // to fill rest of spectrum if longer than number of samples

		for (i=0;i<samples;i++)      // fill spectra with measured raw data samples
		{
			fTrace[chan]->SetBinContent(i+1,(*pl_data16++));    // raw data
		}

		if (samples<codec->SMAX_RAW)
		{
			for (i=samples;i<codec->SMAX_RAW;i++)     // fill rest of spectrum with baseline
			{
				fTrace[chan]->SetBinContent(i+1,first_sample);    // raw data
			}
		}

		pl_data = (UInt_t *) pl_data16;
	}

	//---------------------------------

	//          e_index1=0;
	//          e_index2=0;
	//          e_index3=0;

	//*****************************************************
	// Samples of Energy Filter (whole trapezoid or parts)
	//*****************************************************

	if (energy_data & 0x70000)   //  energy data start index(es) set
	{
		//            if (raw_data)
		//              pl_data = (INTS4 *) pl_data16;

		samples = energy_data & 0xFFFF;

		index_pat=(energy_data & 0x70000)>>16;


		//printf("Index pattrn: %d %d %d\n",e_index3,e_index2,e_index1);
		//printf("Energy Samples: %d\n\n",samples);
		//break;

		//*************************************
		// pat = 001 display whole trapezoid
		//*************************************

		if (index_pat==1)
		{
			if (samples>codec->SMAX_E)
			{
				printf("Energy Samples Trace too long: %d ->return\n",(int)samples);
				break;
			}

			first_e_sample = *pl_data;  // to fill rest of spectrum if longer than number of samples

			for (i=0;i<samples;i++)
			{
				fTrace_e[chan]->SetBinContent(i+1,(*pl_data++));    // trapezoidal shape
			}

			if (samples<codec->SMAX_E)
			{
				for (i=samples;i<codec->SMAX_E;i++)
				{
					fTrace_e[chan]->SetBinContent(i+1,first_e_sample);   // fill rest of spectrum with baseline
				}
			}

			//----------------------------------------------------------------------------------
			// calculate energy from Energy Max Value and First Value of Energy Gate (=Baseline)

			//              energy=*pl_data++;
			//        printf("energy: 0x%x\n",(int)energy);

			//              energy=energy-*pl_data++;    // difference between Emax and Baseline

			//--- like pat = 000 ----------------------------------------------------------------------------
			// NEW 26-02-09 calculate energy from Energy Max Value and First Value of Energy Gate (=Baseline)

			energy=*pl_data++;
			//        printf("energy: 0x%x\n",(int)energy);

			if (fParam->Fill)
				energy=energy-*pl_data++;    // difference between Emax and Baseline
			else
				pl_data++;
		}

		//********************************************************
		// pat = 011, calculate energy from baseline and top data
		//********************************************************

		if (index_pat == 3)    // pat = 011
		{
			base_sum=0;
			for(i=0;i<samples;i++)
				base_sum=base_sum+(long)*pl_data++;

			top_sum=0;
			for(i=0;i<samples;i++)
				top_sum=top_sum+(long)*pl_data++;

			energy=(top_sum-base_sum)/samples;
			//printf("Energy: %d\n",energy);

			pl_data++;   // skip two data longwords
			pl_data++;
		}

		//***********************************************************************
		// pat = 111, calculate energy from baseline (pre and post) and top data
		//***********************************************************************

		if (index_pat == 7)    // pat = 111
		{
			base_sum=0;
			for(i=0;i<samples;i++)
				base_sum=base_sum+(long)*pl_data++;

			top_sum=0;
			for(i=0;i<samples;i++)
				top_sum=top_sum+(long)*pl_data++;

			base2_sum=0;
			for(i=0;i<samples;i++)
				base2_sum=base2_sum+(long)*pl_data++;

			base_sum=(base_sum+base2_sum)>>1;

			energy=(top_sum-base_sum)/samples;
			//printf("Energy: %d\n",energy);

			pl_data++;   // skip two data longwords
			pl_data++;
		}

	}     // end if(energy_data)
	else
	{
		//*************************************************************************
		// pat = 000  no trapezoidal energy data (except max and 1 baseline value)
		//*************************************************************************

		//----------------------------------------------------------------------------------
		// calculate energy from Energy Max Value and First Value of Energy Gate (=Baseline)

		energy=*pl_data++;
		//        printf("energy: 0x%x\n",(int)energy);

		if (fParam->Fill)
			energy=energy-*pl_data++;    // difference between Emax and Baseline
		else
			pl_data++;
	}

	//          fHisto[chan]->Fill(energy>>7);  // 7

	//---------------------------------------------------------------------------------------

#ifdef SIS_SPECTRA
	if((energy>>5)<8192)
	{
		hist_sis[energy>>5]++;

		if(hist_sis[energy>>5]>5000 && !stop_flg)
		{
			stop_flg=1;

			fp=fopen("sis_co60_8k.dat","w");

			for(i=0;i<8192;i++)
				fprintf(fp,"%d\n",hist_sis[i]);

			fclose(fp);
		}
	}
#endif

	//------------------------------

	// last two data words (pileup flag etc. and 0xDEADBEEF trailer)

	trg_cnt = (long) *pl_data++;
	trailer = (long) *pl_data++;

	// fill energy histogram dependend from trg_cnt

	// fHisto[chan]->Fill(energy>>fParam->shift);   // 7

	ft_cnt = (trg_cnt & 0x0F000000)>>24;

	if (ft_cnt == 0)                 // no internal trigger (ADC out of range!?)
	{
		pUnpackEvent->fiGammaQ[chan]=2; // quality
		fPileup[chan]->Fill(200);
	}
	else if (ft_cnt == 1)                 // good event
	{
		fPileup[chan]->Fill(400);
		pUnpackEvent->fiGammaQ[chan]=0; // quality
		pUnpackEvent->fiGammaE[chan]=energy>>fParam->shift;
		pUnpackEvent->fiMultiGamma++;
		fHisto[chan]->Fill(energy>>fParam->shift);
	}
	else if (ft_cnt > 1)                  // pileup
	{
		fPileup[chan]->Fill(600);
		pUnpackEvent->fiGammaQ[chan]=1; // quality
		pUnpackEvent->fiGammaE[chan]=energy>>fParam->shift;
		pUnpackEvent->fiMultiGamma++;
		fHisto[chan]->Fill(energy>>fParam->shift);
	}

}    // end chan loop

return kTRUE;
}

