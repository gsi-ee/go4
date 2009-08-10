//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "CompUnpackProc.h"
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

#include "CompControl.h"
#include "CompParameter.h"
#include "CompPedestals.h"
#include "CompUnpackEvent.h"
#include "CompAnalysis.h"

#define V785_1 7
#define V785_2 8
#define V785_3 9

//***********************************************************
CompUnpackProc::~CompUnpackProc()
{
  cout << "Comp> CompUnpackProc: Processed "<<fiEventsProcessed<<
  " written "<<fiEventsWritten <<
  " last "<<fLastEvent<<endl;
    totalmsec += ((lastmsec-firstmsec)/1000);
     printf("Comp> CompUnpackProc: File%04d sec tot %9d first %9d last %9d dt %5d\n",
	       lastfilenum&0xffff,totalmsec,firstmsec/1000,lastmsec/1000,(lastmsec-firstmsec)/1000);
}
//***********************************************************
CompUnpackProc::CompUnpackProc() :
  TGo4EventProcessor()
{}
//***********************************************************
// this one is used
CompUnpackProc::CompUnpackProc(const char* name) :
   TGo4EventProcessor(name)
{
  cout << "Comp> CompUnpackProc "<<name<<" created" << endl;

  Text_t chis[32];
  Text_t chead[64];
  spillTime0=0;
  spillIndex=0;
  spillOn=kFALSE;
  spillTest=kFALSE;
  memset((void*) spillBins,0, sizeof(spillBins));

  anl=(CompAnalysis *)TGo4Analysis::Instance();

  fParam     = (CompParameter *) anl->CreateParameter("Parameter","Parameters");
  fControl   = (CompControl *)   anl->CreateParameter("Control","Controls");
  fPedestals = (CompPedestals *) anl->CreateParameter("Pedestals","Pedestals");
  codec      = (CompCodec *)     anl->CreateParameter("Codec","Codec");
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
  cout << "Comp> CompUnpackProc: Reset check counters" << endl;
  codec->Cleanup();
  codec->setMap(false); // set true to get printout
  fiEventsProcessed=0;
  fiEventsWritten=0;
  TimeLastsec=0;
  TimeLastmysec=0;
  TimeLastadc=0;
  fLastEvent=0;
  lastfilenum=0;

// Creation of histograms:
  // Optional histograms
  if(fControl->UnpackHisto){
    for(i =0;i<96;i++)
      {
	snprintf(chis,15,"Adc_%02d",i);
	if(i > 63)      snprintf(chead,63,"Mod 3 chan %2d",i-64);
	else if(i > 31) snprintf(chead,63,"Mod 2 chan %2d",i-32);
	else            snprintf(chead,63,"Mod 1 chan %2d",i);
	fAdc[i] = anl->CreateTH1I("Unpack/AllAdc",chis,chead,5000,0.5,5000.5);
      }
    fSystemTime = anl->CreateTH1I ("Unpack","SystemTimeDelta","System delta time","[mysec]","Events",5000,0.5,20000.5);
    fAdcTime    = anl->CreateTH1I ("Unpack","AdcTimeDelta","Adc  delta time","[mysec]","Events",5000,0.5,20000.5);
    fhStopXiLH=anl->CreateTH2I("Unpack","StopXiLvH","Stop X low/high stripe","Low Energy","High Energy","Counts",144,0,144,144,0,144);
    fhStopYiLH=anl->CreateTH2I("Unpack","StopYiLvH","Stop Y low/high stripe","Low Energy","High Energy","Counts",96,0,96,96,0,96);
    fhStopXLH=anl->CreateTH2I("Unpack","StopXLvH","Stop X low/high","Low Energy","High Energy","Counts",500,0,5000,500,0,5000);
    fhStopYLH=anl->CreateTH2I("Unpack","StopYLvH","Stop Y low/high","Low Energy","High Energy","Counts",500,0,5000,500,0,5000);
    fMaxiStopXLg = anl->CreateTH1I ("Unpack","MaxiStopXLgated","Stop XL rel. max. 100-3500",288,0,288);
    fMaxiStopXHg = anl->CreateTH1I ("Unpack","MaxiStopXHgated","Stop XH rel. max. 100-3500",288,0,288);
    fMaxiStopYLg = anl->CreateTH1I ("Unpack","MaxiStopYLgated","Stop YL rel. max. 100-3500",192,0,192);
    fMaxiStopYHg = anl->CreateTH1I ("Unpack","MaxiStopYHgated","Stop YH rel. max. 100-3500",192,0,192);
    fMultiAdc   = anl->CreateTH1I ("Unpack/Multi","MultiAdc","Adc multiplicity",100,0,100);
    fMultiStopXL = anl->CreateTH1I ("Unpack/Multi","MultiStopXL","Stop XL multiplicity",144,0,144);
    fMultiStopXH = anl->CreateTH1I ("Unpack/Multi","MultiStopXH","Stop XH multiplicity",144,0,144);
    fMultiStopYL = anl->CreateTH1I ("Unpack/Multi","MultiStopYL","Stop YL multiplicity",144,0,144);
    fMultiStopYH = anl->CreateTH1I ("Unpack/Multi","MultiStopYH","Stop YH multiplicity",144,0,144);
    fTof        = anl->CreateTH1I ("Unpack","Tof","Adc[80]",1000,0,4000);
    fTofgated   = anl->CreateTH1I ("Unpack","Tofg","Adc[80] gated with TOF",1000,0,4000);
    // test spectrum

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
    for(i=0;i<8;i++){ // 8 rows
	for(k=0;k<4;k++){ // 4 columns
	  anl->SetPicture(M1raw,fAdc[m],i,k,1);
	  anl->SetPicture(M2raw,fAdc[m+32],i,k,1);
	  anl->SetPicture(M3raw,fAdc[m+64],i,k,1);
	  m++;
	}
    }
  }
}
//***********************************************************
void CompUnpackProc::CalcPedestals(){
if(fControl->UnpackHisto){
for(i =0;i<96;i++){
	if(fPedestal)fPedestal->SetBinContent(i,fAdc[i]->GetMean());
	fPedestals->SetPedestals(i,fAdc[i]->GetMean());
}}}
//***********************************************************

//-----------------------------------------------------------
void CompUnpackProc::CompUnpack(CompUnpackEvent* pUP)
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
  //fSize->Fill((fInput->GetDlen()>>1)-5);// is data length in 32bit without headers
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
	  //fSpill->Fill(timediff);
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
    totalmsec += ((lastmsec-firstmsec)/1000);
     printf("Comp> CompUnpackProc: File%04d sec tot %9d first %9d last %9d dt %5d\n",
	       lastfilenum&0xffff,totalmsec,firstmsec/1000,lastmsec/1000,(lastmsec-firstmsec)/1000);
	firstmsec=pUnpackEvent->fiSystemSec;
	lastfilenum=pUnpackEvent->fiFileNumber;
  }
  lastmsec=pUnpackEvent->fiSystemSec;

  //fSystemTime->Fill(fiDeltaSystemTime);
  //fAdcTime->Fill(fiDeltaTime);
// Build Mpx table
// check conditions to select events
//fFilter->Fill(0); // all events
pUnpackEvent->fisTof=codec->isTof();
pUnpackEvent->fisMacro=codec->isMacro();
pUnpackEvent->fisMicro=codec->isMicro();
pUnpackEvent->fisChopper=codec->isChopper();
//   if(codec->isTof())	fFilter->Fill(2);
//   else 					fFilter->Fill(3);
//   if(codec->isChopper())fFilter->Fill(6);
//   else 					fFilter->Fill(7);
//   if(codec->isMacro())	fFilter->Fill(10);
//   else 					fFilter->Fill(11);
//   if(codec->isMicro())	fFilter->Fill(14);
//   else 					fFilter->Fill(15);
  if(fControl->checkTof){
	  fControl->TofChecked++;
	  //fFilter->Fill(1);
	  takeEvent=(fControl->TofMustbe==codec->isTof());
	  if(!takeEvent)return;
	  fControl->TofTrue++;
  }
  if(fControl->checkChopper){
	  fControl->ChopperChecked++;
	  //fFilter->Fill(5);
	  takeEvent=(fControl->ChopperMustbe==codec->isChopper());
	  if(!takeEvent)return;
	  fControl->ChopperTrue++;
  }
  if(fControl->checkMacro){
	  fControl->MacroChecked++;
	  //fFilter->Fill(9);
	  takeEvent=(fControl->MacroMustbe==codec->isMacro());
	  if(!takeEvent)return;
	  fControl->MacroTrue++;
  }
  if(fControl->checkMicro){
	  fControl->MicroChecked++;
	  //fFilter->Fill(13);
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
		  pUnpackEvent->fiMultiAdc++;
		  if(fControl->UnpackHisto){
			  fAdcAllRaw->Fill(codec->getAdc());
			  fContent->Fill(off+codec->getChan());
		  }
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

pUnpackEvent->SetValid(takeEvent); // to store
pUnpackEvent->fisTof=pUnpackEvent->fiAdc[80] > fParam->Adc80TofMin;
// now fill the detector arrays. Low is even, high is odd index in fiAdc
// StopX:
multiL=0;
multiH=0;
for(i=0;i<codec->getStopXnoAdc();i++){
	k=codec->getStopXAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	//cout << "k " << k << " n " << n << " adc "<< H<<endl;
	L=pUnpackEvent->fiAdc[2*k];
	if((L>fParam->AdcThresholdStop)&&(L<fParam->AdcMax)){
		multiL++;
		if(L>pUnpackEvent->fiStopXLhitV){
			pUnpackEvent->fiStopXLhitV=L;
			pUnpackEvent->fiStopXLhitI=n;
		}
	}
	H=pUnpackEvent->fiAdc[2*k+1];
	if((H>fParam->AdcThreshold)&&(H<fParam->AdcMax)){
		multiH++;
		if(H>pUnpackEvent->fiStopXHhitV){
			pUnpackEvent->fiStopXHhitV=H;
			pUnpackEvent->fiStopXHhitI=n;
			//printf("n %d adc %d\n",pUnpackEvent->fiStopXHhitI,pUnpackEvent->fiStopXHhitV);
		}
	}
	pUnpackEvent->fiStopXL[n]=L;
	pUnpackEvent->fiStopXH[n]=H;
	if(fControl->UnpackHisto)fhStopXLH->Fill(L,H);
}//StopX
pUnpackEvent->fiMultiStopXL=multiL;
pUnpackEvent->fiMultiStopXH=multiH;
if(fControl->UnpackHisto) fhStopXiLH->Fill(pUnpackEvent->fiStopXLhitI,pUnpackEvent->fiStopXHhitI);
// search for second maximum
// L=0;
// H=0;
// if(fControl->UnpackHisto)if(multiL>0)fMaxiStopXLg->Fill(144);
// if(fControl->UnpackHisto)if(multiH>0)fMaxiStopXHg->Fill(144);
// if(multiL>1){
//   pUnpackEvent->fiStopXL[pUnpackEvent->fiStopXLhitI]=0;
//   for(i=0;i<144;i++) if(L<pUnpackEvent->fiStopXL[i]){L=pUnpackEvent->fiStopXL[i];k=i;}
//   if(fControl->UnpackHisto)if((L>fParam->AdcThresholdStop)&&(L<fParam->AdcMax))fMaxiStopXLg->Fill(k+144-pUnpackEvent->fiStopXLhitI);
// }
// if(multiH>1){
//   pUnpackEvent->fiStopXH[pUnpackEvent->fiStopXHhitI]=0;
//   for(i=0;i<144;i++) if(H<pUnpackEvent->fiStopXH[i]){H=pUnpackEvent->fiStopXH[i];n=i;}
//   if(fControl->UnpackHisto)if((H>fParam->AdcThreshold)&&(H<fParam->AdcMax))fMaxiStopXHg->Fill(n+144-pUnpackEvent->fiStopXHhitI);
// }
//  if(pUnpackEvent->fiEventNumber == 223528283){
//  if((multiL>1)&&(L>fParam->AdcThresholdStop)&&(L<fParam->AdcMax)){
//  if(k > (pUnpackEvent->fiStopXLhitI+1)) printf("File %03d%04d ev %9d XL m1 %3d v1 %5d m2 %3d v2 %5d d %4d\n",
// 					       pUnpackEvent->fiFileNumber>>16,pUnpackEvent->fiFileNumber&0xffff,
// 					       pUnpackEvent->fiEventNumber,pUnpackEvent->fiStopXLhitI,pUnpackEvent->fiStopXLhitV,
// 					       k,L,pUnpackEvent->fiStopXLhitI-k);
//  else if(k < (pUnpackEvent->fiStopXLhitI-1)) printf("File %03d%04d ev %9d XL m1 %3d v1 %5d m2 %3d v2 %5d d %4d\n",
// 					       pUnpackEvent->fiFileNumber>>16,pUnpackEvent->fiFileNumber&0xffff,
// 					       pUnpackEvent->fiEventNumber,pUnpackEvent->fiStopXLhitI,pUnpackEvent->fiStopXLhitV,
// 					       k,L,pUnpackEvent->fiStopXLhitI-k);
//  }
//  if((multiH>1)&&(H>fParam->AdcThreshold)&&(H<fParam->AdcMax)){
//  if(n > (pUnpackEvent->fiStopXHhitI+1)) printf("File %03d%04d ev %9d XH m1 %3d v1 %5d m2 %3d v2 %5d d %4d\n",
// 					       pUnpackEvent->fiFileNumber>>16,pUnpackEvent->fiFileNumber&0xffff,
// 					       pUnpackEvent->fiEventNumber,pUnpackEvent->fiStopXHhitI,pUnpackEvent->fiStopXHhitV,
// 					       n,H,pUnpackEvent->fiStopXHhitI-n);
//  else if(n < (pUnpackEvent->fiStopXHhitI-1)) printf("File %03d%04d ev %9d XH m1 %3d v1 %5d m2 %3d v2 %5d d %4d\n",
// 					       pUnpackEvent->fiFileNumber>>16,pUnpackEvent->fiFileNumber&0xffff,
// 					       pUnpackEvent->fiEventNumber,pUnpackEvent->fiStopXHhitI,pUnpackEvent->fiStopXHhitV,
// 						    n,H,pUnpackEvent->fiStopXHhitI-n);
//  }
//  }
// StopY:
multiL=0;
multiH=0;
for(i=0;i<codec->getStopYnoAdc();i++){
	k=codec->getStopYAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	L=pUnpackEvent->fiAdc[2*k];
	if((L>fParam->AdcThresholdStop)&&(L<fParam->AdcMax)){
		multiL++;
		if(L>pUnpackEvent->fiStopYLhitV){
			pUnpackEvent->fiStopYLhitV=L;
			pUnpackEvent->fiStopYLhitI=n;
		}
	}
	H=pUnpackEvent->fiAdc[2*k+1];
	if((H>fParam->AdcThreshold)&&(H<fParam->AdcMax)){
		multiH++;
		if(H>pUnpackEvent->fiStopYHhitV){
			pUnpackEvent->fiStopYHhitV=H;
			pUnpackEvent->fiStopYHhitI=n;
		}
	}
	pUnpackEvent->fiStopYL[n]=L;
	pUnpackEvent->fiStopYH[n]=H;
	if(fControl->UnpackHisto)fhStopYLH->Fill(L,H);
}//StopY
pUnpackEvent->fiMultiStopYL=multiL;
pUnpackEvent->fiMultiStopYH=multiH;
if(fControl->UnpackHisto) fhStopYiLH->Fill(pUnpackEvent->fiStopYLhitI,pUnpackEvent->fiStopYHhitI);

// search for second maximum
// L=0;
// H=0;
// if(fControl->UnpackHisto)if(multiL>0)fMaxiStopYLg->Fill(96);
// if(fControl->UnpackHisto)if(multiH>0)fMaxiStopYHg->Fill(96);
// if(multiL>1){
//   pUnpackEvent->fiStopYL[pUnpackEvent->fiStopYLhitI]=0;
//   for(i=0;i<96;i++) if(L<pUnpackEvent->fiStopYL[i]){L=pUnpackEvent->fiStopYL[i];k=i;}
//   if(fControl->UnpackHisto)if((L>fParam->AdcThresholdStop)&&(L<fParam->AdcMax))fMaxiStopYLg->Fill(k+96-pUnpackEvent->fiStopYLhitI);
// }
// if(multiH>1){
//   pUnpackEvent->fiStopYH[pUnpackEvent->fiStopYHhitI]=0;
//   for(i=0;i<96;i++) if(H<pUnpackEvent->fiStopYH[i]){H=pUnpackEvent->fiStopYH[i];n=i;}
//   if(fControl->UnpackHisto)if((H>fParam->AdcThreshold)&&(H<fParam->AdcMax))fMaxiStopYHg->Fill(n+96-pUnpackEvent->fiStopYHhitI);
// }
//  if((multiL>1)&&(L>fParam->AdcThresholdStop)&&(L<fParam->AdcMax)){
//  if(k > (pUnpackEvent->fiStopYLhitI+1)) printf("File %03d%04d ev %9d YL m1 %3d v1 %5d m2 %3d v2 %5d d %4d\n",
// 					       pUnpackEvent->fiFileNumber>>16,pUnpackEvent->fiFileNumber&0xffff,
// 					       pUnpackEvent->fiEventNumber,pUnpackEvent->fiStopYLhitI,pUnpackEvent->fiStopYLhitV,
// 					       k,L,pUnpackEvent->fiStopYLhitI-k);
//  else if(k < (pUnpackEvent->fiStopYLhitI-1)) printf("File %03d%04d ev %9d YL m1 %3d v1 %5d m2 %3d v2 %5d d %4d\n",
// 					       pUnpackEvent->fiFileNumber>>16,pUnpackEvent->fiFileNumber&0xffff,
// 					       pUnpackEvent->fiEventNumber,pUnpackEvent->fiStopYLhitI,pUnpackEvent->fiStopYLhitV,
// 					       k,L,pUnpackEvent->fiStopYLhitI-k);
//  }
//  if((multiH>1)&&(H>fParam->AdcThreshold)&&(H<fParam->AdcMax)){
//  if(n > (pUnpackEvent->fiStopYHhitI+1)) printf("File %03d%04d ev %9d YH m1 %3d v1 %5d m2 %3d v2 %5d d %4d\n",
// 					       pUnpackEvent->fiFileNumber>>16,pUnpackEvent->fiFileNumber&0xffff,
// 					       pUnpackEvent->fiEventNumber,pUnpackEvent->fiStopYHhitI,pUnpackEvent->fiStopYHhitV,
// 					       n,H,pUnpackEvent->fiStopYHhitI-n);
//  else if(n < (pUnpackEvent->fiStopYHhitI-1)) printf("File %03d%04d ev %9d YH m1 %3d v1 %5d m2 %3d v2 %5d d %4d\n",
// 					       pUnpackEvent->fiFileNumber>>16,pUnpackEvent->fiFileNumber&0xffff,
// 					       pUnpackEvent->fiEventNumber,pUnpackEvent->fiStopYHhitI,pUnpackEvent->fiStopYHhitV,
// 						    n,H,pUnpackEvent->fiStopYHhitI-n);
//  }

}// V785 ADCs

if(fControl->UnpackHisto){
  if(spillOn&(pUnpackEvent->fiStopXHhitV>fParam->AlphaMinH))fSpillG->Fill(timediff);
  if(pdata-psubevent) fSizeA->Fill(pdata-psubevent);
}
// optional histogramming
 if(fControl->UnpackHisto){
   for(i=0;i<96;i++) fAdc[i]->Fill(pUnpackEvent->fiAdc[i]);
   fMultiAdc->Fill(pUnpackEvent->fiMultiAdc);
   fMultiStopXL->Fill(pUnpackEvent->fiMultiStopXL);
   fMultiStopXH->Fill(pUnpackEvent->fiMultiStopXH);
   fMultiStopYL->Fill(pUnpackEvent->fiMultiStopYL);
   fMultiStopYH->Fill(pUnpackEvent->fiMultiStopYH);
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

