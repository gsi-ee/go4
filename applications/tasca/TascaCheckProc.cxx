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

  anl=(TascaAnalysis *)TGo4Analysis::Instance();

  fControl = (TascaControl *)   anl->CreateParameter("Control","Controls");
  fParam   = (TascaParameter *) anl->CreateParameter("Parameter","Parameters");
  gROOT->ProcessLine(".x setparam.C()");
  gROOT->ProcessLine(".x setcontrol.C()");

  if(fControl->CheckHisto){
  fAlphaGammaL=anl->CreateTH2D("Check","AlphaGammaL","Energies","Alpha [Kev]","Gamma [Kev]","Hits",1000,1,30000,1000,1,2500);
  fAlphaBackL=anl->CreateTH2D("Check","AlphaBackL","Energies","Alpha [Kev]","Back [Kev]","Hits",1000,1,30000,1000,1,10000);
  fAlphaVetoL=anl->CreateTH2D("Check","AlphaVetoL","Energies","Alpha [Kev]","Veto [Kev]","Hits",1000,1,30000,1000,1,4000);
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
fInput  = (TascaCaliEvent*) GetInputEvent();
poutevt->fisTof=fInput->fisTof;
poutevt->fisChopper=fInput->fisChopper;
poutevt->fisMicro=fInput->fisMicro;
poutevt->fisMacro=fInput->fisMacro;
poutevt->fiEventNumber=fInput->fiEventNumber;
poutevt->fiStopXLhitI=fInput->fiStopXLhitI;
poutevt->fiStopXHhitI=fInput->fiStopXHhitI;
poutevt->fiStopYLhitI=fInput->fiStopYLhitI;
poutevt->fiStopYHhitI=fInput->fiStopYHhitI;
poutevt->fiBackHhitI=fInput->fiBackHhitI;
poutevt->fiBackLhitI=fInput->fiBackLhitI;
poutevt->fiVetoHhitI=fInput->fiVetoHhitI;
poutevt->fiVetoLhitI=fInput->fiVetoLhitI;
// value of maximum hit, if we had more than one hit
poutevt->ffStopXLhitV=fInput->ffStopXLhitV;
poutevt->ffStopXHhitV=fInput->ffStopXHhitV;
poutevt->ffStopYLhitV=fInput->ffStopYLhitV;
poutevt->ffStopYHhitV=fInput->ffStopYHhitV;
poutevt->ffBackHhitV =fInput->ffBackHhitV;
poutevt->ffBackLhitV =fInput->ffBackLhitV;
poutevt->ffVetoHhitV =fInput->ffVetoHhitV;
poutevt->ffVetoLhitV =fInput->ffVetoLhitV;
poutevt->fiSystemMysec=fInput->fiSystemMysec;
poutevt->fiDeltaTime=fInput->fiDeltaTime;
poutevt->fiDeltaSystemTime=fInput->fiDeltaSystemTime;
poutevt->fiGammaMysec = fInput->fiGammaMysec;
for(Int_t i=0;i<7;i++){
	poutevt->ffGammaKev[i]   = fInput->ffGammaKev[i];
	poutevt->fiGammaChannelTime[i]   = fInput->fiGammaChannelTime[i];
}

Bool_t YH=fadcKevH->Test(fInput->ffStopYHhitV);
Bool_t YL=fadcKevL->Test(fInput->ffStopYLhitV);
Bool_t XL=fadcKevL->Test(fInput->ffStopXLhitV);
Bool_t XH=fadcKevH->Test(fInput->ffStopXHhitV);

if(fControl->CheckHisto){
if(YL) fStopLE[fInput->fiStopYLhitI%48]->Fill(fInput->fiStopXLhitI,fInput->ffStopXLhitV);

if(YH) fStopHE[fInput->fiStopYHhitI%48]->Fill(fInput->fiStopXHhitI,fInput->ffStopXHhitV);

if(XH & YH & XL & YL) fStopXY->Fill(fInput->fiStopXHhitI,fInput->fiStopYHhitI%48);

fAlphaBackL->Fill(fInput->ffStopXLhitV,fInput->ffBackLhitV);

fAlphaVetoL->Fill(fInput->ffStopXLhitV,fInput->ffVetoLhitV);

for(Int_t i=0;i<7;i++) fAlphaGammaL->Fill(fInput->ffStopXLhitV,fInput->ffGammaKev[i]);
} // fControl->CheckHisto

poutevt->SetValid(kTRUE);       // events are not stored until kTRUE is set

} // BuildCalEvent
