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
#include "TascaUnpackEvent.h"

//***********************************************************
TascaUnpackProc::TascaUnpackProc() :
  TGo4EventProcessor()
{
}
//***********************************************************
// this one is used in TascaUnpackFact.cxx
TascaUnpackProc::TascaUnpackProc(const char* name) :
   TGo4EventProcessor(name)
{
  cout << "**** TascaUnpackProc: Create" << endl;

  Text_t chis[16];
  Text_t chead[64];

  codec = new TascaCodec();
  evcount=0;

  //// init user analysis objects:

  fParam1   = (TascaParameter *)   GetParameter("TascaPar1");
  fParam2   = (TascaParameter *)   GetParameter("TascaPar2");
//  fParam1->PrintParameter(0,0);
//  fParam2->PrintParameter(0,0);

  // Creation of histograms:
  if(GetHistogram("Mod1/M1chan01")==0)
    {
      for(i =0;i<32;i++)
      {
		snprintf(chis,15,"M1chan%02d",i+1);
		snprintf(chead,63,"Mod 08 chan %2d",i+1);
		fM1Ch[i] = new TH1I (chis,chead,5000,-0.5,4999.5);
		AddHistogram(fM1Ch[i],"Mod1");
		snprintf(chis,15,"M2chan%02d",i+1);
		snprintf(chead,63,"Mod 10 chan %2d",i+1);
		fM2Ch[i] = new TH1I (chis,chead,5000,-0.5,4999.5);
		AddHistogram(fM2Ch[i],"Mod2");
		snprintf(chis,15,"M3chan%02d",i+1);
		snprintf(chead,63,"Mod 12 chan %2d",i+1);
		fM3Ch[i] = new TH1I (chis,chead,5000,-0.5,4999.5);
		AddHistogram(fM3Ch[i],"Mod3");
      }
		fPed1 = new TH1I ("M1ped","Mod 08 pedestals",32,-0.5,31.5);
		fPed2 = new TH1I ("M2ped","Mod 10 pedestals",32,-0.5,31.5);
		fPed3 = new TH1I ("M3ped","Mod 12 pedestals",32,-0.5,31.5);
		AddHistogram(fPed1);
		AddHistogram(fPed2);
		AddHistogram(fPed3);

    }
  else // got them from autosave file
    {
      for(i =0;i<32;i++)
   {
		snprintf(chis,15,"Mod1/M1chan%02d",i+1);
		fM1Ch[i]=(TH1I*)GetHistogram(chis);
		snprintf(chis,15,"Mod2/M2chan%02d",i+1);
		fM2Ch[i]=(TH1I*)GetHistogram(chis);
		snprintf(chis,15,"Mod3/M3chan%02d",i+1);
		fM3Ch[i]=(TH1I*)GetHistogram(chis);
   }
      fPed1=(TH1I*)GetHistogram("M1ped");
      fPed2=(TH1I*)GetHistogram("M2ped");
      fPed3=(TH1I*)GetHistogram("M3ped");

      cout << "Unpack: Restored histograms from autosave" << endl;
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
      cout << "Unpack: Restored conditions from autosave" << endl;
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
    	  M1raw->Pic(i,k)->AddH1(fM1Ch[m]);
          M1raw->Pic(i,k)->SetStatsAttr(0.1,0.6,0.4,0.75,100);
          M1raw->Pic(i,k)->SetTitleAttr(0.1,0.75,0.7,0.9);
    	  M2raw->Pic(i,k)->AddH1(fM2Ch[m]);
          M2raw->Pic(i,k)->SetStatsAttr(0.1,0.6,0.4,0.75,100);
          M2raw->Pic(i,k)->SetTitleAttr(0.1,0.75,0.7,0.9);
    	  M3raw->Pic(i,k)->AddH1(fM3Ch[m]);
          M3raw->Pic(i,k)->SetStatsAttr(0.1,0.6,0.4,0.75,100);
          M3raw->Pic(i,k)->SetTitleAttr(0.1,0.75,0.7,0.9);
    	  m++;
      }
    }
  else
    {
      M1raw = GetPicture("M1raw");
      M2raw = GetPicture("M2raw");
      M3raw = GetPicture("M3raw");
      cout << "Unpack: Restored pictures from autosave" << endl;
    }

  fWinCon1->Enable();
}
//***********************************************************
TascaUnpackProc::~TascaUnpackProc()
{
for(Int_t i =0;i<32;i++)
{
cout <<
fM1Ch[i]->GetName() << ":" << fM1Ch[i]->GetMean() << " " <<
fM2Ch[i]->GetName() << ":" << fM2Ch[i]->GetMean() << " " <<
fM3Ch[i]->GetName() << ":" << fM3Ch[i]->GetMean()
<< endl;
}
}
//***********************************************************
void TascaUnpackProc::savePedestals(){
for(Int_t i =0;i<32;i++)
{
	fPed1->SetBinContent(i,fM1Ch[i]->GetMean());
	fPed2->SetBinContent(i,fM2Ch[i]->GetMean());
	fPed3->SetBinContent(i,fM3Ch[i]->GetMean());
}
}
//***********************************************************

//-----------------------------------------------------------
void TascaUnpackProc::TascaUnpack(TascaUnpackEvent* poutevt)
{
  TGo4MbsSubEvent* psubevt;
  Int_t crate, address, channels, header;
  Int_t lwords;
  Int_t *pdata;
  Int_t latches=0;
  Int_t adcs=0;
  Int_t patt0,patt1,patt2,patt3;
  Int_t lat,lat0,lat1,lat2,lat3;
  poutevt->SetValid(kFALSE); // not to store
  fInput    = (TGo4MbsEvent* ) GetInputEvent(); // from this
  if(fInput == 0) return;
  if(fInput->GetTrigger()==14) return;
  if(fInput->GetTrigger()==15) return;
  //cout << "Event=" << fInput->GetCount() << endl;
  fInput->ResetIterator();
  psubevt = fInput->NextSubEvent();
  pdata=psubevt->GetDataField();
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

  // first module
while(adcs > 0){
  adcs--;
  header=*pdata++;
  codec->setValue(header);
  if(codec->isHeader()){
	  crate=codec->getCrate();
	  channels=codec->getCnt();
	  address=codec->getAddress();
	  //cout << "    Crate=" << crate << " addr=" << address << " channels=" << channels << endl;
	  if(address == 8){
		  for(Int_t i=0;i<channels;i++){
			  codec->setValue(*pdata++);
			  fM1Ch[codec->getChan()]->Fill(codec->getAdc());
			  poutevt->fiMod1[codec->getChan()]=codec->getAdc();
	  }}
	  else if(address == 10){
		  for(Int_t i=0;i<channels;i++){
			  codec->setValue(*pdata++);
			  fM2Ch[codec->getChan()]->Fill(codec->getAdc());
			  poutevt->fiMod2[codec->getChan()]=codec->getAdc();
	  }}
	  else if(address == 12){
		  for(Int_t i=0;i<channels;i++){
			  codec->setValue(*pdata++);
			  fM3Ch[codec->getChan()]->Fill(codec->getAdc());
			  poutevt->fiMod3[codec->getChan()]=codec->getAdc();
	  }}
//	  for(Int_t i=0;i<channels;i++){
//		  codec->setValue(*pdata++);
//		  cout
//		  << "      addr=" << codec->getAddress()
//		  << " chan=" << codec->getChan()
//			  << " adc=" << codec->getAdc()
//			  << " Un=" << codec->isUnder()
//			  << " Ov=" << codec->isOver()
//			  << endl;
//	  }
	  pdata++; // skip EOB
  } else if(codec->isEob()){
	  //cout << "    EOB " << endl;
  } else if(!codec->isValid()){
	  //cout << "    No data " << endl;
  } else {
	  //cout << "    No header found " << header << endl;
  }
  } // loop over ADCs
evcount++;
if(evcount%100 != 0)savePedestals();
poutevt->SetValid(kTRUE); // to store
}
