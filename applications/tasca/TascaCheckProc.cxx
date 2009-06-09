//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaCheckProc.h"

#include "Riostream.h"

#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TMultiGraph.h"

#include "TGo4WinCond.h"

#include "TascaCheckEvent.h"
#include "TascaCaliEvent.h"
#include "TascaControl.h"
#include "TascaParameter.h"
#include "TGo4WinCond.h"
#include "TascaAnalysis.h"

//***********************************************************
TascaCheckProc::TascaCheckProc()
  : TGo4EventProcessor(),fInput(0)
{
}
//***********************************************************
// this one is used in TascaCheckFact
TascaCheckProc::TascaCheckProc(const char* name) :
  TGo4EventProcessor(name)
{
  cout << "Tasca> TascaCheckProc: Create" << endl;
  //// init user analysis objects:

  Text_t chis[32];
  Text_t chead[64];

  TimeLastgamma=0;
  TimeLastmysec=0;
  TimeLastadc=0;
  anl=(TascaAnalysis *)TGo4Analysis::Instance();

  fControl = (TascaControl *)   anl->CreateParameter("Control","Controls");
  fParam   = (TascaParameter *) anl->CreateParameter("Parameter","Parameters");
  gROOT->ProcessLine(".x setparam.C()");
  gROOT->ProcessLine(".x setcontrol.C()");

  fTime      = anl->CreateTH1I ("Check","Time","Time diff",20000,0,20000000);
  if(fControl->CheckHisto){
  fAlphaGammaL=anl->CreateTH2D("Check","AlphaGammaL","Energies","Alpha [Kev]","Gamma [Kev]","Hits",1000,0,30000,1000,0,2500);
  fAlphaBackL=anl->CreateTH2D("Check","AlphaBackL","Energies","Alpha [Kev]","Back [Kev]","Hits",1000,0,30000,1000,0,10000);
  fAlphaVetoL=anl->CreateTH2D("Check","AlphaVetoL","Energies","Alpha [Kev]","Veto [Kev]","Hits",1000,0,30000,1000,0,4000);
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
    fgammaKev= (TGo4WinCond *)anl->CreateCondition("Check","gammaKev",0,kTRUE,0,2000);
    fwinEVR     = (TGo4WinCond *)anl->CreateCondition("Check","EvrH",0,kTRUE,7000,20000);
    fwinAlpha   = (TGo4WinCond *)anl->CreateCondition("Check","AlphaL",0,kTRUE,9800,10200);
    fwinAlpha1  = (TGo4WinCond *)anl->CreateCondition("Check","Alpha1L",0,kTRUE,9700,10100);
    fwinAlpha2  = (TGo4WinCond *)anl->CreateCondition("Check","Alpha2L",0,kTRUE,8970,9370);
    fwinFission1= (TGo4WinCond *)anl->CreateCondition("Check","Fission1H",0,kTRUE,60000,220000);
    fwinFission2= (TGo4WinCond *)anl->CreateCondition("Check","Fission2H",0,kTRUE,60000,220000);
    fwinBack    = (TGo4WinCond *)anl->CreateCondition("Check","BackH",0,kTRUE,10000,80000);
    fwinEVR->Enable();
    fwinFission1->Enable();
    fwinFission2->Enable();
    fwinBack->Enable();
    fwinAlpha->Enable();
    fwinAlpha1->Enable();
    fwinAlpha2->Enable();
    fadcKevH->Enable();
    fadcKevL->Enable();
    fgammaKev->Enable();
  }
//***********************************************************
TascaCheckProc::~TascaCheckProc()
{
	  cout << "Tasca> TascaCheckProc: Delete" << endl;
}
//***********************************************************

//-----------------------------------------------------------
void TascaCheckProc::TascaEventCheck(TascaCheckEvent* poutevt)
{
Bool_t takeEvent=kFALSE;
fInput  = (TascaCaliEvent*) GetInputEvent();

if(fControl->CheckHisto){
	Bool_t YH=fadcKevH->Test(fInput->ffStopYHhitV);
	Bool_t YL=fadcKevL->Test(fInput->ffStopYLhitV);
	Bool_t XL=fadcKevL->Test(fInput->ffStopXLhitV);
	Bool_t XH=fadcKevH->Test(fInput->ffStopXHhitV);
	if(YL) fStopLE[fInput->fiStopYLhitI%48]->Fill(fInput->fiStopXLhitI,fInput->ffStopXLhitV);

	if(YH) fStopHE[fInput->fiStopYHhitI%48]->Fill(fInput->fiStopXHhitI,fInput->ffStopXHhitV);

	if(XH & YH & XL & YL) fStopXY->Fill(fInput->fiStopXHhitI,fInput->fiStopYHhitI%48);

	fAlphaBackL->Fill(fInput->ffStopXLhitV,fInput->ffBackLhitV);

	fAlphaVetoL->Fill(fInput->ffStopXLhitV,fInput->ffVetoLhitV);

	for(Int_t i=0;i<7;i++) fAlphaGammaL->Fill(fInput->ffStopXLhitV,fInput->ffGammaKev[i]);
} // fControl->CheckHisto

// Filter out events
if(fInput->fisTof){
	if(fInput->fisMacro&fwinEVR->Test(fInput->ffStopXHhitV)){
		takeEvent=kTRUE;
		poutevt->fisEvr=kTRUE;
	}
} else {
	if(fwinAlpha->Test(fInput->ffStopXLhitV)) {takeEvent=kTRUE;poutevt->fisAlpha=kTRUE;}
	if(fwinAlpha1->Test(fInput->ffStopXLhitV)){takeEvent=kTRUE;poutevt->fisAlpha=kTRUE;}
	if(fwinAlpha2->Test(fInput->ffStopXLhitV)){takeEvent=kTRUE;poutevt->fisAlpha=kTRUE;}
	if(fwinFission1->Test(fInput->ffStopXHhitV)&
			fwinBack->Test(fInput->ffBackHhitV)){
		takeEvent=kTRUE;
		poutevt->fisFission=kTRUE;
	}
}
poutevt->SetValid(kFALSE);       // events are not stored until kTRUE is set
if(takeEvent){
	poutevt->fisTof=fInput->fisTof;
	poutevt->fisChopper=fInput->fisChopper;
	poutevt->fisMicro=fInput->fisMicro;
	poutevt->fisMacro=fInput->fisMacro;
	poutevt->fisVeto=fInput->fisVeto;
	poutevt->fiEventNumber=fInput->fiEventNumber;
	poutevt->fiStopXLhitI=fInput->fiStopXLhitI;
	poutevt->fiStopXHhitI=fInput->fiStopXHhitI;
	poutevt->fiStopYLhitI=fInput->fiStopYLhitI;
	poutevt->fiStopYHhitI=fInput->fiStopYHhitI;
	poutevt->fiBackHhitI=fInput->fiBackHhitI;
	poutevt->fiBackLhitI=fInput->fiBackLhitI;
	poutevt->fiVetoHhitI=fInput->fiVetoHhitI;
	poutevt->fiVetoLhitI=fInput->fiVetoLhitI;
	poutevt->fiMultiStopXL=fInput->fiMultiStopXL;
	poutevt->fiMultiStopXH=fInput->fiMultiStopXH;
	poutevt->fiMultiStopYL=fInput->fiMultiStopYL;
	poutevt->fiMultiStopYH=fInput->fiMultiStopYH;
	poutevt->fiMultiBackH=fInput->fiMultiBackH;
	poutevt->fiMultiBackL=fInput->fiMultiBackL;
	poutevt->fiMultiVetoH=fInput->fiMultiVetoH;
	poutevt->fiMultiVetoL=fInput->fiMultiVetoL;
	// value of maximum hit, if we had more than one hit
	poutevt->ffStopXLhitV=fInput->ffStopXLhitV;
	poutevt->ffStopXHhitV=fInput->ffStopXHhitV;
	poutevt->ffStopYLhitV=fInput->ffStopYLhitV;
	poutevt->ffStopYHhitV=fInput->ffStopYHhitV;
	poutevt->ffBackHhitV =fInput->ffBackHhitV;
	poutevt->ffBackLhitV =fInput->ffBackLhitV;
	poutevt->ffVetoHhitV =fInput->ffVetoHhitV;
	poutevt->ffVetoLhitV =fInput->ffVetoLhitV;
	poutevt->fiTimeStamp=fInput->fiTimeStamp;
	poutevt->fiSystemMysec=fInput->fiSystemMysec;
	poutevt->fiGammaMysec = fInput->fiGammaMysec;
	for(Int_t i=0;i<7;i++){
		poutevt->ffGammaKev[i]   = fInput->ffGammaKev[i];
	}
// Calculate incremental time
	if(fInput->fiTimeStamp<TimeLastadc)
		 poutevt->fiDeltaTime=0xFFFFFFFF-TimeLastadc+fInput->fiTimeStamp+1;
	else poutevt->fiDeltaTime=fInput->fiTimeStamp-TimeLastadc;
	TimeLastadc=fInput->fiTimeStamp;
	if(poutevt->fiSystemMysec<TimeLastmysec)
		 poutevt->fiDeltaSystemTime=0xFFFFFFFF-TimeLastmysec+poutevt->fiSystemMysec+1;
	else poutevt->fiDeltaSystemTime=poutevt->fiSystemMysec-TimeLastmysec;
	TimeLastmysec=poutevt->fiSystemMysec;
	if(poutevt->fiGammaMysec<TimeLastgamma)
		 poutevt->fiDeltaGammaTime=0xFFFFFFFF-TimeLastgamma+poutevt->fiGammaMysec+1;
	else poutevt->fiDeltaGammaTime=poutevt->fiGammaMysec-TimeLastgamma;
	TimeLastgamma=poutevt->fiGammaMysec;
	fTime->Fill(poutevt->fiDeltaTime);
	poutevt->SetValid(kTRUE);       // events are not stored until kTRUE is set
}
return;
} // BuildCalEvent
