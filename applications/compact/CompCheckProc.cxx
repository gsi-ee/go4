//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "CompCheckProc.h"

#include "Riostream.h"

#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TMultiGraph.h"

#include "TGo4WinCond.h"

#include "CompCheckEvent.h"
#include "CompUnpackEvent.h"
#include "CompControl.h"
#include "CompParameter.h"
#include "CompCaliFitter.h"
#include "CompCalibration.h"
#include "TGo4WinCond.h"
#include "CompAnalysis.h"

//***********************************************************
CompCheckProc::~CompCheckProc()
{
	  cout << "Comp> CompCheckProc:  Processed "<<fiEventsProcessed<<
	  " written "<<fiEventsWritten <<
	  " last "<<fLastEvent<<endl;
}
//***********************************************************
CompCheckProc::CompCheckProc()
  : TGo4EventProcessor(),fInput(0)
{
}
//***********************************************************
// this one is used in CompCheckFact
CompCheckProc::CompCheckProc(const char* name) :
  TGo4EventProcessor(name),fInput(0),fLastEvent(-1)
  {
	cout << "Comp> CompCheckProc "<<name<<" created" << endl;
	//// init user analysis objects:

	Text_t chis[32];
	Text_t chead[64];

	TimeLastmysec=0;
	TimeLastadc=0;
	fiEventsProcessed=0;
	fiEventsWritten=0;
	anl=(CompAnalysis *)TGo4Analysis::Instance();

	fParam   = (CompParameter *) anl->CreateParameter("Parameter","Parameters");
	fControl = (CompControl *)   anl->CreateParameter("Control","Controls");
	fCaliFit     = (CompCaliFitter *)  anl->CreateParameter("CaliFitter","CaliFitter");
	fCalibration = (CompCalibration *) anl->CreateParameter("Calibration","Calibration");
	fCaliGraph   = (TGraph *)           anl->CreateObject("Graph","CaliGraph",24);
	// setup calibration
	fCaliFit->Setup(fCaliGraph);
	gROOT->ProcessLine(".x setcontrol.C()");
	gROOT->ProcessLine(".x setparam.C()");
	gROOT->ProcessLine(".x setcali.C()"); // en-disable calibration

	if(fControl->CheckHisto){
		fhStopXLH=anl->CreateTH2I("Cali/Sum","StopXLH","Stop X low/high","Low Energy [Kev]","High Energy [Kev]","Counts",4000,0,30000,4000,0,30000);
		fhStopYLH=anl->CreateTH2I("Cali/Sum","StopYLH","Stop Y low/high","Low Energy [Kev]","High Energy [Kev]","Counts",4000,0,30000,4000,0,30000);
		fhdStopXL=anl->CreateTH1D("Cali/Sum","StopXL", "StopX all low",144,0,144);
		fhdStopXH=anl->CreateTH1D("Cali/Sum","StopXH", "StopX all high",144,0,144);
		fhdStopYL=anl->CreateTH1D("Cali/Sum","StopYL", "StopY all low",96,0,96);
		fhdStopYH=anl->CreateTH1D("Cali/Sum","StopYH", "StopY all high",96,0,96);
		fhdStopXLsum=anl->CreateTH1D("Cali/Sum","StopXLsum", "StopX sum low",4000,0.5,4000.5);
		fhdStopXHsum=anl->CreateTH1D("Cali/Sum","StopXHsum", "StopX sum high",4000,0.5,4000.5);
		fhdStopYLsum=anl->CreateTH1D("Cali/Sum","StopYLsum", "StopY sum low",4000,0.5,4000.5);
		fhdStopYHsum=anl->CreateTH1D("Cali/Sum","StopYHsum", "StopY sum high",4000,0.5,4000.5);
		if(fCalibration->UseCalibration){
			fhdStopXLsum->GetXaxis()->SetLimits(1,30001);
			fhdStopYLsum->GetXaxis()->SetLimits(1,30001);
			fhdStopXHsum->GetXaxis()->SetLimits(200,300200);
			fhdStopYHsum->GetXaxis()->SetLimits(200,300200);
			fhdStopXLsum->GetXaxis()->SetTitle("Energy [Kev]");
			fhdStopXHsum->GetXaxis()->SetTitle("Energy [Kev]");
			fhdStopYLsum->GetXaxis()->SetTitle("Energy [Kev]");
			fhdStopYHsum->GetXaxis()->SetTitle("Energy [Kev]");
		}
		for(i=0;i<144;i++){
			snprintf(chis,15,"StopXL_%03d",i);
			snprintf(chead,63,"StopX Low %03d",i);
			fhStopXL[i]=anl->CreateTH1I("Cali/StopXL",chis,chead,4000,0.5,4000.5);
			snprintf(chis,15,"StopXH_%03d",i);
			snprintf(chead,63,"StopX High %03d",i);
			fhStopXH[i]=anl->CreateTH1I("Cali/StopXH",chis,chead,4000,0.5,4000.5);
			if(fCalibration->UseCalibration){
				fhStopXL[i]->GetXaxis()->SetLimits(1,30001);
				fhStopXH[i]->GetXaxis()->SetLimits(200,300200);
				fhStopXL[i]->GetXaxis()->SetTitle("Energy [Kev]");
				fhStopXH[i]->GetXaxis()->SetTitle("Energy [kev]");
			}
		}
		for(i=0;i<96;i++){
			snprintf(chis,15,"StopYL_%03d",i);
			snprintf(chead,63,"StopY Low %03d",i);
			fhStopYL[i]=anl->CreateTH1I("Cali/StopYL",chis,chead,4000,0.5,4000.5);
			snprintf(chis,15,"StopYH_%03d",i);
			snprintf(chead,63,"StopY High %03d",i);
			fhStopYH[i]=anl->CreateTH1I("Cali/StopYH",chis,chead,4000,0.5,4000.5);
			fhStopYH[i]->GetXaxis()->SetTitle("Channels");
			fhStopYH[i]->GetYaxis()->SetTitle("Counts");
			fhStopYL[i]->GetXaxis()->SetTitle("Channels");
			fhStopYL[i]->GetYaxis()->SetTitle("Counts");
			if(fCalibration->UseCalibration){
				fhStopYL[i]->GetXaxis()->SetLimits(1,30001);
				fhStopYH[i]->GetXaxis()->SetLimits(200,300200);
				fhStopYL[i]->GetXaxis()->SetTitle("Energy [Kev]");
				fhStopYH[i]->GetXaxis()->SetTitle("Energy [Kev]");
			}
		}
		// pictures rows, columns
		StopX[0] = anl->CreatePicture("Cali","pStopXL0","Stop X low",8,6);
		StopX[1] = anl->CreatePicture("Cali","pStopXL1","Stop X low",8,6);
		StopX[2] = anl->CreatePicture("Cali","pStopXL2","Stop X low",8,6);
		StopX[3] = anl->CreatePicture("Cali","pStopXH0","Stop X high",8,6);
		StopX[4] = anl->CreatePicture("Cali","pStopXH1","Stop X high",8,6);
		StopX[5] = anl->CreatePicture("Cali","pStopXH2","Stop X high",8,6);

		StopY[0] = anl->CreatePicture("Cali","pStopYL0","Stop Y low",8,6);
		StopY[1] = anl->CreatePicture("Cali","pStopYL1","Stop Y low",8,6);
		StopY[2] = anl->CreatePicture("Cali","pStopYH0","Stop Y high",8,6);
		StopY[3] = anl->CreatePicture("Cali","pStopYH1","Stop Y high",8,6);
		anl->SetPicture(Sum,fhdStopXLsum,0,0,1);
		anl->SetPicture(Sum,fhdStopYLsum,1,0,1);
		anl->SetPicture(Sum,fhdStopXHsum,0,1,1);
		anl->SetPicture(Sum,fhdStopYHsum,1,1,1);

		Hits     = anl->CreatePicture("Cali","pDetectorHits","Summary detector hits",4,2);

		anl->SetPicture(Hits,fhdStopXL,0,0,1);
		anl->SetPicture(Hits,fhdStopYL,1,0,1);
		anl->SetPicture(Hits,fhdStopXH,0,1,1);
		anl->SetPicture(Hits,fhdStopYH,1,1,1);

		Int_t m=0;
		// enlarge stats box and position in [0:1] coordinates
		// show only Mean value (ROOT manual "Statistics Display")
		for(i=0;i<8;i++){ // 8 rows
			for(k=0;k<6;k++){ // 6 columns
				anl->SetPicture(StopX[0],fhStopXL[m],i,k,1);
				anl->SetPicture(StopX[1],fhStopXL[m+48],i,k,1);
				anl->SetPicture(StopX[2],fhStopXL[m+96],i,k,1);
				anl->SetPicture(StopX[3],fhStopXH[m],i,k,1);
				anl->SetPicture(StopX[4],fhStopXH[m+48],i,k,1);
				anl->SetPicture(StopX[5],fhStopXH[m+96],i,k,1);

				anl->SetPicture(StopY[0],fhStopYL[m],i,k,1);
				anl->SetPicture(StopY[1],fhStopYL[m+48],i,k,1);
				anl->SetPicture(StopY[2],fhStopYH[m],i,k,1);
				anl->SetPicture(StopY[3],fhStopYH[m+48],i,k,1);
				m++;
			}}
		m=0;
		fadcKevH = (TGo4WinCond *)anl->CreateCondition("Check","adcKevH",0,kTRUE,0,300000);
		fadcKevL = (TGo4WinCond *)anl->CreateCondition("Check","adcKevL",0,kTRUE,0,30000);
		fTime      = anl->CreateTH1I ("Check","Time","Time diff",20000,0,20000000);
		fStopXY=anl->CreateTH2D("Check","StopXYhits","Hit counters","X position [stripe]","Y position [stripe]","Hits",144,0,144,48,0,48);
		for(i=0;i<48;i++){
			snprintf(chis,15,"XH_%03d",i);
			snprintf(chead,63,"Stop X High %03d",i);
			fStopHE[i]=anl->CreateTH2D("Check/StopHE",chis,chead,"X position [stripe]","Energy [Kev]","Counts",144,0,144,200,0,300000);
			snprintf(chis,15,"XL_%03d",i);
			snprintf(chead,63,"Stop X Low %03d",i);
			fStopLE[i]=anl->CreateTH2D("Check/StopLE",chis,chead,"X position [stripe]","Energy [Kev]","Counts",144,0,144,200,0,30000);
		}
	}// fControl->CheckHisto
	// Creation of conditions (check if restored from auto save file):
	fadcKevH = (TGo4WinCond *)anl->CreateCondition("Check","adcKevH",0,kTRUE,0,300000);
	fadcKevL = (TGo4WinCond *)anl->CreateCondition("Check","adcKevL",0,kTRUE,0,30000);
	fwinEvr     = (TGo4WinCond *)anl->CreateCondition("Check","EvrH",0,kTRUE,0,1);
	fwinAlpha   = (TGo4WinCond *)anl->CreateCondition("Check","AlphaL",0,kTRUE,0,1);
	fwinAlpha1  = (TGo4WinCond *)anl->CreateCondition("Check","Alpha1L",0,kTRUE,0,1);
	fwinAlpha2  = (TGo4WinCond *)anl->CreateCondition("Check","Alpha2L",0,kTRUE,0,1);
	fwinFission1= (TGo4WinCond *)anl->CreateCondition("Check","Fission1H",0,kTRUE,0,1);
	fwinFission2= (TGo4WinCond *)anl->CreateCondition("Check","Fission2H",0,kTRUE,0,1);
	fwinBack    = (TGo4WinCond *)anl->CreateCondition("Check","BackH",0,kTRUE,0,1);
  }
//***********************************************************

//-----------------------------------------------------------
void CompCheckProc::CompEventCheck(CompCheckEvent* poutevt)
{
Bool_t takeEvent=kFALSE;
Float_t alpha;
poutevt->SetValid(takeEvent);       // events are not stored until kTRUE is set
fInput  = (CompUnpackEvent*) GetInputEvent();
fiEventsProcessed++;
// Process only if event is valid
//cout <<"Chk: "<<fInput->fiEventNumber<< endl;
if(!fInput->IsValid()) return;
if(fLastEvent<0)
	  cout <<"      Checker: First event "<<fInput->fiEventNumber<<endl;
fLastEvent=fInput->fiEventNumber;

// Fill output event
  poutevt->ffStopXLhitV=fCalibration->CalibrateStopXL(fInput->fiStopXLhitV,fInput->fiStopXLhitI);
  poutevt->ffStopXHhitV=fCalibration->CalibrateStopXH(fInput->fiStopXHhitV,fInput->fiStopXHhitI);
  poutevt->ffStopYLhitV=fCalibration->CalibrateStopYL(fInput->fiStopYLhitV,fInput->fiStopYLhitI);
  poutevt->ffStopYHhitV=fCalibration->CalibrateStopYH(fInput->fiStopYHhitV,fInput->fiStopYHhitI);

  if(poutevt->ffStopXLhitV>fParam->AlphaMaxL)poutevt->ffStopXLhitV=poutevt->ffStopXHhitV;
  if(poutevt->ffStopYLhitV>fParam->AlphaMaxL)poutevt->ffStopYLhitV=poutevt->ffStopYHhitV;


  if(fControl->CheckHisto){
	Bool_t YH=fadcKevH->Test(poutevt->ffStopYHhitV);
	Bool_t YL=fadcKevL->Test(poutevt->ffStopYLhitV);
	Bool_t XL=fadcKevL->Test(poutevt->ffStopXLhitV);
	Bool_t XH=fadcKevH->Test(poutevt->ffStopXHhitV);
	if(YL) fStopLE[fInput->fiStopYLhitI%48]->Fill(fInput->fiStopXLhitI,poutevt->ffStopXLhitV);
	if(YH) fStopHE[fInput->fiStopYHhitI%48]->Fill(fInput->fiStopXHhitI,poutevt->ffStopXHhitV);
	if(XH & YH & XL & YL) fStopXY->Fill(fInput->fiStopXHhitI,fInput->fiStopYHhitI%48);
	  fhStopXLH->Fill(poutevt->ffStopXLhitV,poutevt->ffStopXHhitV);
	  fhStopYLH->Fill(poutevt->ffStopYLhitV,poutevt->ffStopYHhitV);

} // fControl->CheckHisto

// Filter out events
if(fInput->fisTof){
	if(fInput->fisMacro&fwinEvr->Test(poutevt->ffStopXHhitV)){
		takeEvent=kTRUE;
		poutevt->fisEvr=kTRUE;
	}
} else {
  alpha=poutevt->ffStopXLhitV;
	if(fwinAlpha->Test(alpha)) {takeEvent=kTRUE;poutevt->fisAlpha=kTRUE;}
	if(fwinAlpha1->Test(alpha)){takeEvent=kTRUE;poutevt->fisAlpha=kTRUE;}
	if(fwinAlpha2->Test(alpha)){takeEvent=kTRUE;poutevt->fisAlpha=kTRUE;}
	if(fwinFission1->Test(poutevt->ffStopXHhitV))
		  {takeEvent=kTRUE;poutevt->fisFission=kTRUE;}
}
if(takeEvent){
	poutevt->fisTof=fInput->fisTof;
	poutevt->fisChopper=fInput->fisChopper;
	poutevt->fisMicro=fInput->fisMicro;
	poutevt->fisMacro=fInput->fisMacro;
	poutevt->fisVeto=fInput->fisVeto;
	poutevt->fiFileNumber=fInput->fiFileNumber;
	poutevt->fiEventNumber=fInput->fiEventNumber;
	poutevt->fiStopXLhitI=fInput->fiStopXLhitI;
	poutevt->fiStopXHhitI=fInput->fiStopXHhitI;
	poutevt->fiStopYLhitI=fInput->fiStopYLhitI;
	poutevt->fiStopYHhitI=fInput->fiStopYHhitI;
 	poutevt->fiMultiStopXL=fInput->fiMultiStopXL;
 	poutevt->fiMultiStopXH=fInput->fiMultiStopXH;
	// value of maximum hit, if we had more than one hit
	poutevt->fiTimeStamp=fInput->fiTimeStamp;
	poutevt->fiSystemmsec=fInput->fiSystemSec;
	poutevt->fiSystemMysec=fInput->fiSystemMysec;
// Calculate incremental time
	if(fInput->fiTimeStamp<TimeLastadc)
		 poutevt->fiDeltaTime=0xFFFFFFFF-TimeLastadc+fInput->fiTimeStamp+1;
	else poutevt->fiDeltaTime=fInput->fiTimeStamp-TimeLastadc;
	TimeLastadc=fInput->fiTimeStamp;
	if(fInput->fiSystemMysec<TimeLastmysec)
		 poutevt->fiDeltaSystemTime=0xFFFFFFFF-TimeLastmysec+fInput->fiSystemMysec+1;
	else poutevt->fiDeltaSystemTime=fInput->fiSystemMysec-TimeLastmysec;
	TimeLastmysec=fInput->fiSystemMysec;
	if(fControl->CheckHisto)fTime->Fill(poutevt->fiDeltaTime);
	poutevt->SetValid(kTRUE);       // events are not stored until kTRUE is set
	fiEventsWritten++;
}
return;
} // BuildCalEvent
