//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaAnlProc.h"

#include "Riostream.h"

#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TMultiGraph.h"

#include "TGo4WinCond.h"

#include "TascaAnlEvent.h"
#include "TascaCaliEvent.h"
#include "TascaControl.h"
#include "TascaParameter.h"
#include "TGo4WinCond.h"
#include "TascaAnalysis.h"

//***********************************************************
TascaAnlProc::TascaAnlProc()
  : TGo4EventProcessor(),fInput(0)
{
}
//***********************************************************
// this one is used in TascaAnlFact
TascaAnlProc::TascaAnlProc(const char* name) :
  TGo4EventProcessor(name)
{
  cout << "Tasca> TascaAnlProc: Create" << endl;
  //// init user analysis objects:

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
  fAlphaGammaL=anl->CreateTH2D("Anl","AlphaGammaL","Energies",1000,0,30000,1000,0,2500);
  fAlphaGammaL->GetXaxis()->SetTitle("Alpha [Kev]");
  fAlphaGammaL->GetYaxis()->SetTitle("Gamma [Kev]");
  fAlphaGammaL->GetZaxis()->SetTitle("Hits");

  fAlphaBackL=anl->CreateTH2D("Anl","AlphaBackL","Energies",1000,0,30000,1000,0,10000);
  fAlphaBackL->GetXaxis()->SetTitle("Alpha [Kev]");
  fAlphaBackL->GetYaxis()->SetTitle("Back [Kev]");
  fAlphaBackL->GetZaxis()->SetTitle("Hits");

  fStopXY=anl->CreateTH2D("Anl","StopXYhits","Hit counters",144,0,144,48,0,48);
  fStopXY->GetXaxis()->SetTitle("X position [stripe]");
  fStopXY->GetYaxis()->SetTitle("Y position [stripe]");
  fStopXY->GetZaxis()->SetTitle("Hits");
  for(i=0;i<48;i++){
    snprintf(chis,15,"XH_%03d",i);
    snprintf(chead,63,"Stop X High %03d",i);
    fStopHE[i]=anl->CreateTH2D("Anl/StopHE",chis,chead,144,0,144,200,0,300000);
    fStopHE[i]->GetXaxis()->SetTitle("X position [stripe]");
    fStopHE[i]->GetYaxis()->SetTitle("Energy [Kev]");
    snprintf(chis,15,"XL_%03d",i);
    snprintf(chead,63,"Stop X Low %03d",i);
    fStopLE[i]=anl->CreateTH2D("Anl/StopLE",chis,chead,144,0,144,200,0,30000);
    fStopLE[i]->GetXaxis()->SetTitle("X position [stripe]");
    fStopLE[i]->GetYaxis()->SetTitle("Energy [Kev]");
  }
// Creation of conditions (check if restored from auto save file):
    fadcKevH = (TGo4WinCond *)anl->CreateCondition("Anl","adcKevH",0,kTRUE,0,300000);
    fadcKevL = (TGo4WinCond *)anl->CreateCondition("Anl","adcKevL",0,kTRUE,0,30000);
    fgammaKev= (TGo4WinCond *)anl->CreateCondition("Anl","gammaKev",0,kTRUE,0,2000);
    fadcKevH->Enable();
    fadcKevL->Enable();
    fgammaKev->Enable();
}
//***********************************************************
TascaAnlProc::~TascaAnlProc()
{
	  cout << "Tasca> TascaAnlProc: Delete" << endl;
}
//***********************************************************

//-----------------------------------------------------------
void TascaAnlProc::TascaEventAnalysis(TascaAnlEvent* poutevt)
{
fInput  = (TascaCaliEvent*) GetInputEvent();
Bool_t YH=fadcKevH->Test(fInput->ffStopYHhitV);
Bool_t YL=fadcKevL->Test(fInput->ffStopYLhitV);
Bool_t XL=fadcKevL->Test(fInput->ffStopXLhitV);
Bool_t XH=fadcKevH->Test(fInput->ffStopXHhitV);

if(YL) fStopLE[fInput->fiStopYLhitI%48]->Fill(fInput->fiStopXLhitI,fInput->ffStopXLhitV);

if(YH) fStopHE[fInput->fiStopYHhitI%48]->Fill(fInput->fiStopXHhitI,fInput->ffStopXHhitV);

if(XH & YH & XL & YL) fStopXY->Fill(fInput->fiStopXHhitI,fInput->fiStopYHhitI%48);

 fAlphaBackL->Fill(fInput->ffStopXLhitV,fInput->ffBackLhitV);

 for(Int_t i=0;i<7;i++) fAlphaGammaL->Fill(fInput->ffStopXLhitV,fInput->ffGammaKev[i]);

//  cout << "Yi "<< fInput->fiStopYHhitI%48
//  << " Xi "<<fInput->fiStopXHhitI
//  << " v "  << fInput->ffStopXHhitV<< endl;

poutevt->SetValid(kFALSE);       // events are not stored until kTRUE is set

} // BuildCalEvent
