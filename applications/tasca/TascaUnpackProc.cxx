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

  fControl   = (TascaControl *) GetParameter("Controls");
  if(fControl==0){
	  fControl = new TascaControl("Controls");
	  AddParameter(fControl);
  }
  fParam   = (TascaParameter *) GetParameter("Parameters");
  if(fParam==0){
	  fParam = new TascaParameter("Parameters");
	  AddParameter(fParam);
  }
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
  // reset counters
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
  evcount=0;
  gammaTimeLast=0;
  secTimeLast=0;
  mysecTimeLast=0;
  adcTimeLast=0;

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
  for(i =0;i<96;i++)
  {
	snprintf(chis,15,"Adc_%02d",i);
	if(i > 63)      snprintf(chead,63,"Mod 3 chan %2d",i-64);
	else if(i > 31) snprintf(chead,63,"Mod 2 chan %2d",i-32);
	else            snprintf(chead,63,"Mod 1 chan %2d",i);
	fAdc[i] = anl->CreateTH1I("Unpack/AllAdc",chis,chead,5000,0.5,5000.5);
  }
  for(i =0;i<8;i++)
  {
		snprintf(chis,15,"GammaE_%d",i);
		snprintf(chead,63,"Gamma E raw %d",i);
		fGammaE[i] = anl->CreateTH1I ("Unpack/GammaE",chis,chead,9000,0.5,9000.5);
		snprintf(chis,15,"GammaT_%d",i);
		snprintf(chead,63,"Gamma T raw %d",i);
		fGammaT[i] = anl->CreateTH1I ("Unpack/GammaT",chis,chead,5000,0.5,5000.5);
  }
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

	fFilter    = anl->CreateTH1I ("Unpack","Filter","Tof,chopper,macro,micro",17,0,17);
	fPedestal  = anl->CreateTH1I ("Unpack","Pedestals","Pedestals",96,-0.5,95.5);
	fContent   = anl->CreateTH1I ("Unpack","Contents","Contents",96,-0.5,95.5);
	fTree      = anl->CreateTH1I (0,"Tree","Leaf",5000,0.5,5000.5);
	fTime      = anl->CreateTH1I (0,"Time","Time diff",5000,0.5,5000.5);
	fAdcAllRaw = anl->CreateTH1I ("Unpack","AdcAllRaw","All adc raw",5000,0.5,5000.5);
	fAdcAllCal = anl->CreateTH1I ("Unpack","AdcAllCal","All adc cal",5000,0.5,5000.5);

// pictures rows, columns
    Geraw = anl->CreatePicture("Unpack","pGamma","Gamma raw",8,2);
    M1raw = anl->CreatePicture("Unpack","pV785_1","Module 7",8,4);
    M2raw = anl->CreatePicture("Unpack","pV785_2","Module 9",8,4);
    M3raw = anl->CreatePicture("Unpack","pV785_3","Module 11",8,4);
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
	if(fPedestal)fPedestal->SetBinContent(i,fAdc[i]->GetMean());
	fPedestals->SetPedestals(i,fAdc[i]->GetMean());
}}
//***********************************************************

//-----------------------------------------------------------
void TascaUnpackProc::TascaUnpack(TascaUnpackEvent* pUP)
{
  TGo4MbsSubEvent* psubevt;
  UInt_t crate, address, channels, header, off;
  UInt_t lwords;
  UInt_t *pdata,*pbehind;
  UInt_t latches=0;
  UInt_t adcs=0;
  UInt_t patt0,patt1,patt2,patt3;
  UInt_t lat,lat0,lat1,lat2,lat3;
  UInt_t  iStopXLhits=0,iStopXHhits=0,
		  iStopYLhits=0,iStopYHhits=0,
		  iBackLhits=0,iBackHhits=0,
		  iVetoLhits=0,iVetoHhits=0;
  Bool_t takeEvent=kFALSE;
  pUnpackEvent=pUP;
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
  pbehind=pdata+lwords;
  pUnpackEvent->SetValid(takeEvent); // not to store
  // look for system time stamp
  if(*pdata == 0xaffeaffe){
	  pdata++;
	  pUnpackEvent->fiSystemSec=*pdata++;
	  pUnpackEvent->fiSystemMysec=*pdata++;
  }
  // get number of latches and number of V785 ADCs
  latches = *pdata & 0xFFFF;
  adcs    = (*pdata & 0xFFFF0000) >> 16;

  // Get V785 data
  if(adcs > 0){
  pdata++;
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
  fTime->Fill((patt0-adcTimeLast)/100000);
  adcTimeLast=patt0;
  secTimeLast=pUnpackEvent->fiSystemSec;
  mysecTimeLast=pUnpackEvent->fiSystemMysec;
// Build Mpx table
  codec->setMpxIndex(lat0,lat1,lat2,lat3);
// check conditions to select events
fFilter->Fill(0); // all events
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
  takeEvent=kTRUE;
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
		  fAdc[off+codec->getChan()]->Fill(codec->getAdc());
		  fAdcAllRaw->Fill(codec->getAdc());
		  fContent->Fill(off+codec->getChan());
	  }
	  pdata++; // skip EOB
	  pUnpackEvent->SetValid(kTRUE); // to store
  } else if(codec->isEob()){
	  //cout << "    EOB " << endl;
  } else if(!codec->isValid()){
      //cout << "    No data " << endl;
  } else {
	  //cout << "    No header found " << header << endl;
  }
  } // loop over ADCs

if(fPedestals->Calibrate)	{
	for(i=0;i<96;i++){
	pUnpackEvent->fiAdc[i]=pUnpackEvent->fiAdc[i]+
	(UInt_t )(fPedestals->ffOffset-fPedestals->ffPedestals[i]);
	fAdcAllCal->Fill(pUnpackEvent->fiAdc[i]);
}}

// now fill the detector arrays. Low is even, high is odd index in fiAdc
// StopY
for(i=0;i<codec->getStopYnoAdc();i++){
	k=codec->getStopYAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	if((pUnpackEvent->fiAdc[2*k]>0)&(iStopYLhits<4)){
		pUnpackEvent->fiStopYLhits[iStopYLhits]=n;
		iStopYLhits++;
		if(pUnpackEvent->fiAdc[2*k]>pUnpackEvent->fiStopYLhitV){
			pUnpackEvent->fiStopYLhitV=pUnpackEvent->fiAdc[2*k];
			pUnpackEvent->fiStopYLhitI=n;
		}
	}
	if((pUnpackEvent->fiAdc[2*k+1]>0)&(iStopYHhits<4)){
		pUnpackEvent->fiStopYHhits[iStopYHhits]=n;
		iStopYHhits++;
		if(pUnpackEvent->fiAdc[2*k+1]>pUnpackEvent->fiStopYHhitV){
			pUnpackEvent->fiStopYHhitV=pUnpackEvent->fiAdc[2*k+1];
			pUnpackEvent->fiStopYHhitI=n;
		}
	}
	pUnpackEvent->fiStopYL[n]=pUnpackEvent->fiAdc[2*k];
	pUnpackEvent->fiStopYH[n]=pUnpackEvent->fiAdc[2*k+1];
}//StopY
//printf("YHi %d ",pUnpackEvent->fiStopYHhitI);
// StopX
for(i=0;i<codec->getStopXnoAdc();i++){
	k=codec->getStopXAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	//cout << "k " << k << " n " << n << " adc "<< pUnpackEvent->fiAdc[2*k+1]<<endl;
	if((pUnpackEvent->fiAdc[2*k]>0)&(iStopXLhits<4)){
		pUnpackEvent->fiStopXLhits[iStopXLhits]=n;
		iStopXLhits++;
		if(pUnpackEvent->fiAdc[2*k]>pUnpackEvent->fiStopXLhitV){
			pUnpackEvent->fiStopXLhitV=pUnpackEvent->fiAdc[2*k];
			pUnpackEvent->fiStopXLhitI=n;
		}
	}
	if((pUnpackEvent->fiAdc[2*k+1]>0)&(iStopXHhits<4)){
		pUnpackEvent->fiStopXHhits[iStopXHhits]=n;
		iStopXHhits++;
		if(pUnpackEvent->fiAdc[2*k+1]>pUnpackEvent->fiStopXHhitV){
			pUnpackEvent->fiStopXHhitV=pUnpackEvent->fiAdc[2*k+1];
			pUnpackEvent->fiStopXHhitI=n;
			//printf("n %d adc %d\n",pUnpackEvent->fiStopXHhitI,pUnpackEvent->fiStopXHhitV);
		}
	}
	pUnpackEvent->fiStopXL[n]=pUnpackEvent->fiAdc[2*k];
	pUnpackEvent->fiStopXH[n]=pUnpackEvent->fiAdc[2*k+1];
}//StopX
// Back
for(i=0;i<codec->getBacknoAdc();i++){
	k=codec->getBackAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	if((pUnpackEvent->fiAdc[2*k]>0)&(iBackLhits<4)){
		pUnpackEvent->fiBackLhits[iBackLhits]=n;
		iBackLhits++;
		if(pUnpackEvent->fiAdc[2*k]>pUnpackEvent->fiBackLhitV){
			pUnpackEvent->fiBackLhitV=pUnpackEvent->fiAdc[2*k];
			pUnpackEvent->fiBackLhitI=n;
		}
	}
	if((pUnpackEvent->fiAdc[2*k+1]>0)&(iBackHhits<4)){
		pUnpackEvent->fiBackHhits[iBackHhits]=n;
		iBackHhits++;
		if(pUnpackEvent->fiAdc[2*k+1]>pUnpackEvent->fiBackHhitV){
			pUnpackEvent->fiBackHhitV=pUnpackEvent->fiAdc[2*k+1];
			pUnpackEvent->fiBackHhitI=n;
		}
	}
	pUnpackEvent->fiBackL[n]=pUnpackEvent->fiAdc[2*k];
	pUnpackEvent->fiBackH[n]=pUnpackEvent->fiAdc[2*k+1];
}// Back
// Veto
for(i=0;i<codec->getVetonoAdc();i++){
	k=codec->getVetoAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	if((pUnpackEvent->fiAdc[2*k]>0)&(iVetoLhits<4)){
		pUnpackEvent->fiVetoLhits[iVetoLhits]=n;
		iVetoLhits++;
		if(pUnpackEvent->fiAdc[2*k]>pUnpackEvent->fiVetoLhitV){
			pUnpackEvent->fiVetoLhitV=pUnpackEvent->fiAdc[2*k];
			pUnpackEvent->fiVetoLhitI=n;
		}
	}
	if((pUnpackEvent->fiAdc[2*k+1]>0)&(iVetoHhits<4)){
		pUnpackEvent->fiVetoHhits[iVetoHhits]=n;
		iVetoHhits++;
		if(pUnpackEvent->fiAdc[2*k+1]>pUnpackEvent->fiVetoHhitV){
			pUnpackEvent->fiVetoHhitV=pUnpackEvent->fiAdc[2*k+1];
			pUnpackEvent->fiVetoHhitI=n;
		}
	}
	pUnpackEvent->fiVetoL[n]=pUnpackEvent->fiAdc[2*k];
	pUnpackEvent->fiVetoH[n]=pUnpackEvent->fiAdc[2*k+1];
}//Veto
}// V785 ADCs
// follows Sis3302
if(pdata != pbehind){
  pdata++; // skip first tag word
  DecodeGamma(pdata,pbehind);
  pUnpackEvent->SetValid(takeEvent); // to store
  for(i=0;i<codec->SCHANNELS;i++){
	fGammaE[i]->Fill(pUnpackEvent->fiGammaE[i]);
	fGammaT[i]->Fill(pUnpackEvent->fiGammaT[i]-gammaTimeLast);
  }
  gammaTimeLast=pUnpackEvent->fiGammaE[7];
  return;
}
} // check for valid event
evcount++;
if(!fPedestals->Calibrate){
	if(evcount >= 10000) {
	CalcPedestals();
	evcount=0;
	//throw TGo4EventEndException(this);
	}}
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
energy,
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

	pl_data++;  // skip two timestamp longwords
	pUnpackEvent->fiGammaT[chan]=*pl_data++;
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
		fHisto[chan]->Fill(energy>>fParam->shift);
	}
	else if (ft_cnt > 1)                  // pileup
	{
		fPileup[chan]->Fill(600);
		pUnpackEvent->fiGammaQ[chan]=1; // quality
		pUnpackEvent->fiGammaE[chan]=energy>>fParam->shift;
		fHisto[chan]->Fill(energy>>fParam->shift);
	}

}    // end chan loop

return kTRUE;
}

