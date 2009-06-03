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
#include "TascaEventStack.h"
#include "TascaEvent.h"
#include "TascaCheckEvent.h"
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
  fEventStack=new TascaEventStack(10);
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
fInput  = (TascaCheckEvent*) GetInputEvent();
if(fEventStack->used<fEventStack->entries){ // fill stack slots
	((TascaEvent *)fEventStack->At(fEventStack->used))->Copy(fInput);
	fEventStack->used++;
}
else { // remove first and put to end
	fEvent=(TascaEvent *) fEventStack->First();
	fEvent->Copy(fInput);
	fEventStack->Remove(fEvent);
	fEventStack->AddLast(fEvent);
	fEvent=(TascaEvent *) fEventStack->First();
	// loop over all events of stack
	while(fEvent){
		fEvent=(TascaEvent *) fEventStack->After(fEvent);
	}
}
poutevt->SetValid(kFALSE);       // events are not stored until kTRUE is set

} // BuildCalEvent
