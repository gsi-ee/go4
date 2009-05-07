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
  // histograms only if required
if(fControl->AnlHisto){
  for(i=0;i<48;i++){
	snprintf(chis,15,"XH_%03d",i);
	snprintf(chead,63,"Stop X High %03d",i);
    fStopHE[i]=anl->CreateTH2D("Anl/StopHE",chis,chead,144,0,144,200,0,200);
	snprintf(chis,15,"XL_%03d",i);
	snprintf(chead,63,"Stop X Low %03d",i);
    fStopLE[i]=anl->CreateTH2D("Anl/StopLE",chis,chead,144,0,144,200,0,200);
}}}
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
  if(fControl->AnlHisto){
	  if((fInput->fiStopYLhitI>0)&(fInput->fiStopYLhitI<96))
	  fStopLE[fInput->fiStopYLhitI%48]->Fill(fInput->fiStopXLhitI,fInput->ffStopXLhitV);
	  if((fInput->fiStopYHhitI>0)&(fInput->fiStopYHhitI<96))
	  fStopHE[fInput->fiStopYHhitI%48]->Fill(fInput->fiStopXHhitI,fInput->ffStopXHhitV);
	//  cout << "Yi "<< fInput->fiStopYHhitI%48
	//  << " Xi "<<fInput->fiStopXHhitI
	//  << " v "  << fInput->ffStopXHhitV<< endl;
  }
  poutevt->SetValid(kFALSE);       // events are not stored until kTRUE is set


} // BuildCalEvent
