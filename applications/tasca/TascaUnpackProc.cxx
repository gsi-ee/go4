#include "TascaUnpackProc.h"

#include "Riostream.h"
#include <time.h>

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

  Text_t chis[16];
  Text_t chead[64];

  codec = new TascaCodec();
  codec->setMap(false);
  evcount=0;

  //// init user analysis objects:

  fParPed   = (TascaPedestals *)   GetParameter("TascaPedestals");

  // Creation of histograms:
  if(GetHistogram("AllAdc/Adc00")==0)
    {
      for(i =0;i<96;i++)
      {
		snprintf(chis,15,"Adc%02d",i);
		if(i > 63)      snprintf(chead,63,"Mod 12 chan %2d",i-64);
		else if(i > 31) snprintf(chead,63,"Mod 10 chan %2d",i-32);
		else            snprintf(chead,63,"Mod 08 chan %2d",i);
		fAdc[i] = new TH1I (chis,chead,5000,-0.5,4999.5);
		AddHistogram(fAdc[i],"AllAdc");
      }
	fPedestal = new TH1I ("Pedestals","Pedestals",96,-0.5,95.5);
	AddHistogram(fPedestal);
	fContent = new TH1I ("Contents","Contents",96,-0.5,95.5);
	AddHistogram(fContent);

    }
  else // got them from autosave file
    {
      for(i =0;i<96;i++)
      {
		snprintf(chis,15,"AllAdc/Adc%02d",i);
		fAdc[i]=(TH1I*)GetHistogram(chis);
      }
      fPedestal=(TH1I*)GetHistogram("Pedestals");
      fContent=(TH1I*)GetHistogram("Contents");

      cout << "Tasca> TascaUnpackProc: Restored histograms from autosave" << endl;
    }
  // Creation of conditions:
  if(GetAnalysisCondition("wincon1")==0)
    {
      fWinCon1= new TGo4WinCond("wincon1");
      fWinCon1->SetValues(50,2000);
      fWinCon1->Disable(true); // return always true

    }
  else // got them from autosave file
    {
      fWinCon1  = (TGo4WinCond*)  GetAnalysisCondition("wincon1");
      cout << "Tasca> TascaUnpackProc: Restored conditions from autosave" << endl;
    }

// pictures
  if (GetPicture("M1raw")==0)
    {
      M1raw = new TGo4Picture("M1raw","Module 8",8,4);
      M2raw = new TGo4Picture("M2raw","Module 10",8,4);
      M3raw = new TGo4Picture("M3raw","Module 12",8,4);
      AddPicture(M1raw);
      AddPicture(M2raw);
      AddPicture(M3raw);
      Int_t m=0;
      // 4 columns, 8 rows
      // enlarge stats box and position in [0:1] coordinates
      // show only Mean value (ROOT manual "Statistics Display")
      for(i=0;i<8;i++)for(k=0;k<4;k++){
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
      }
    }
  else
    {
      M1raw = GetPicture("M1raw");
      M2raw = GetPicture("M2raw");
      M3raw = GetPicture("M3raw");
      cout << "Tasca> TascaUnpackProc: Restored pictures from autosave" << endl;
    }

  fWinCon1->Enable();
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
	fParPed->SetPedestals(i,fAdc[i]->GetMean());
}}
//***********************************************************

//-----------------------------------------------------------
void TascaUnpackProc::TascaUnpack(TascaUnpackEvent* poutevt)
{
  TGo4MbsSubEvent* psubevt;
  UInt_t crate, address, channels, header, off;
  UInt_t lwords;
  UInt_t *pdata;
  UInt_t latches=0;
  UInt_t adcs=0;
  UInt_t patt0,patt1,patt2,patt3;
  UInt_t lat,lat0,lat1,lat2,lat3;

  poutevt->SetValid(kFALSE); // not to store
  fInput    = (TGo4MbsEvent* ) GetInputEvent(); // from this
  if(fInput == 0) return;
  if(fInput->GetTrigger()==14) return;
  if(fInput->GetTrigger()==15) return;
  if(fInput->GetDlen() == 36) return;
  //cout << "Event=" << fInput->GetCount() << endl;
  fInput->ResetIterator();
  psubevt = fInput->NextSubEvent();
  pdata=(UInt_t *)psubevt->GetDataField();
  lwords= psubevt->GetIntLen();
  // get number of latches and number of ADCs
  latches = *pdata & 0xFFFF;
  adcs = (*pdata++ & 0xFFFF0000) >> 16;
  patt0=*pdata++; // latch base + 0
  lat=*pdata++; //
  lat0=*pdata++; // latch 0

  patt1=*pdata++; // latch base + 1
  lat1=*pdata++; // latch 1
  patt2=*pdata++; // latch base + 2
  lat2=*pdata++; // latch 2
  patt3=*pdata++; // latch base + 3
  lat3=*pdata++; // latch 3

  //cout << "  ADCs=" << adcs << " latches=" << latches << endl;

// Build Mpx table
  codec->setMpxIndex(lat0,lat1,lat2,lat3);
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
		  poutevt->fiAdc[off+codec->getChan()]=codec->getAdc();
		  fAdc[off+codec->getChan()]->Fill(codec->getAdc());
		  fContent->Fill(off+codec->getChan());
	  }
	  pdata++; // skip EOB
	  poutevt->SetValid(kTRUE); // to store
  } else if(codec->isEob()){
	  //cout << "    EOB " << endl;
  } else if(!codec->isValid()){
	  cout << "    No data " << endl;
  } else {
	  cout << "    No header found " << header << endl;
  }
  } // loop over ADCs


// now fill the detector arrays
// StopX
for(i=0;i<codec->getStopXnoAdc();i++){
	k=codec->getStopXAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	poutevt->fiStopXL[n]=poutevt->fiAdc[2*k];
	poutevt->fiStopXH[n]=poutevt->fiAdc[2*k+1];
}
// StopY
for(i=0;i<codec->getStopYnoAdc();i++){
	k=codec->getStopYAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	poutevt->fiStopYL[n]=poutevt->fiAdc[2*k];
	poutevt->fiStopYH[n]=poutevt->fiAdc[2*k+1];
}
// Back
for(i=0;i<codec->getBacknoAdc();i++){
	k=codec->getBackAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	poutevt->fiBackL[n]=poutevt->fiAdc[2*k];
	poutevt->fiBackH[n]=poutevt->fiAdc[2*k+1];
}
// Veto
for(i=0;i<codec->getVetonoAdc();i++){
	k=codec->getVetoAdc(i); // ADC channel index, low or high
	n=codec->getIndex(k);    // from that get stripe index
	poutevt->fiVetoL[n]=poutevt->fiAdc[2*k];
	poutevt->fiVetoH[n]=poutevt->fiAdc[2*k+1];
}
evcount++;
if(evcount%10000 != 0) CalcPedestals();
}
