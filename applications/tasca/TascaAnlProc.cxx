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
  gROOT->ProcessLine(".x setparam.C()");
  gROOT->ProcessLine(".x setcontrol.C()");
  fEventStack=new TascaEventStack(fParam->EventStackSize);
  fStackIter = new TListIter(fEventStack);
  fEventIter = new TListIter(fEventStack);
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
// Note that we have already filled the list with preallocated events.
// Therefore the iterator runs over all entries.
fEvent=(TascaEvent *)fStackIter->Next();
if(fEvent){ // fill stack slots
	fEvent->Copy(fInput); // copy event data into event entry
	stackfilled=kTRUE;
}
else { // remove first and put to end
	if(stackfilled){
		stackfilled=kFALSE; // to avoid further printout
		cout<<" Event stack filled "<<fEventStack->entries<< " entries"<<endl;
	}
	fEvent=(TascaEvent *) fEventStack->First(); // the oldest one
	fEvent->Copy(fInput); // current
	fEventStack->Remove(fEvent);
	fEventStack->AddLast(fEvent); // latest at the end
	// loop over all events of stack
	fEvent=(TascaEvent *) fEventStack->First();
	// this takes about 7 mysec per 100 entries
	fEventIter->Reset();
	while(fEvent=(TascaEvent *)fEventIter->Next()){
		// cout<<fEvent->fiEventNumber<<" ";
	}
	// cout<<endl;
}
poutevt->SetValid(kFALSE);       // events are not stored until kTRUE is set

} // BuildCalEvent
