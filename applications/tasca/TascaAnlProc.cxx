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
  : TGo4EventProcessor(),fInput(0),fFissions(0),fAlphas(0),fEvrs(0)
{
}
//***********************************************************
// this one is used in TascaAnlFact
TascaAnlProc::TascaAnlProc(const char* name) :
  TGo4EventProcessor(name),fInput(0),fFissions(0),fAlphas(0),fEvrs(0)
{
  cout << "Tasca> TascaAnlProc "<<name<<" created" << endl;
  //// init user analysis objects:


  anl=(TascaAnalysis *)TGo4Analysis::Instance();

  fControl = (TascaControl *)   anl->CreateParameter("Control","Controls");
  fParam   = (TascaParameter *) anl->CreateParameter("Parameter","Parameters");
  fwinEVR     = (TGo4WinCond *)anl->CreateCondition("Check","EvrH",0,kTRUE,0,1);
  fwinAlpha   = (TGo4WinCond *)anl->CreateCondition("Check","AlphaL",0,kTRUE,0,1);
  fwinAlpha1  = (TGo4WinCond *)anl->CreateCondition("Check","Alpha1L",0,kTRUE,0,1);
  fwinAlpha2  = (TGo4WinCond *)anl->CreateCondition("Check","Alpha2L",0,kTRUE,0,1);
  fwinFission1= (TGo4WinCond *)anl->CreateCondition("Check","Fission1H",0,kTRUE,0,1);
  fwinFission2= (TGo4WinCond *)anl->CreateCondition("Check","Fission2H",0,kTRUE,0,1);
  fwinBack    = (TGo4WinCond *)anl->CreateCondition("Check","BackH",0,kTRUE,0,1);
  gROOT->ProcessLine(".x setparam.C()");
  gROOT->ProcessLine(".x setcontrol.C()");
  fEventStack=new TascaEventStack(fParam->EventStackSize);
  fStackIter = new TListIter(fEventStack);
  fEventIter = new TListIter(fEventStack);
  fEvent=(TascaEvent *) fEventStack->First();
  fStackfilled=kFALSE;
  fFirstEvent=0;
  fAlphaFound=kFALSE;
  }
//***********************************************************
TascaAnlProc::~TascaAnlProc()
{
	  cout << "Tasca> TascaAnlProc: Delete" << endl;
	  cout << "      Fissions "<<fFissions << endl;
	  cout << "      Alphas   "<<fAlphas << endl;
	  cout << "      EVRs     "<<fEvrs << endl;
}
//***********************************************************

//-----------------------------------------------------------
void TascaAnlProc::TascaEventAnalysis(TascaAnlEvent* poutevt)
{
poutevt->SetValid(kFALSE);       // events are not stored until kTRUE is set
fInput  = (TascaCheckEvent*) GetInputEvent();
// Process only if event is valid
if(!fInput->IsValid()) return;
// Note that we have already filled the list with preallocated events.
// Therefore the iterator runs over all entries.
if(fInput->fisFission)fFissions++;
if(fInput->fisAlpha)fAlphas++;
if(fInput->fisEvr)fEvrs++;
if(fFirstEvent==0)fFirstEvent=fInput->fiEventNumber;

// we assume that fEvent is a valid slot
// at the beginning this is slot 1
if(fInput->fisFission){
	fAlphaFound=kFALSE;
	fEvent->Copy(fInput); // copy event data into event entry
	fEventStack->used++;
	fFissionEvent=fEvent;
	fStackEvent=fEvent;
	fTimeDiff=0;
	printf("Fission %3d: %8d  Mev %6.2f Back %6.2f\n",fFissions,
			fFissionEvent->fiEventNumber,
			fFissionEvent->ffStopXHhitV/1000.,
			fFissionEvent->ffBackHhitV/1000.
			);
	while((fStackEvent=(TascaEvent *) fEventStack->Before(fStackEvent))!=0){
		fTimeDiff += fStackEvent->fiDeltaTime;
		if(fTimeDiff < fParam->Fission1Tmax){ // in alpha time window
		if(fStackEvent->fisAlpha){
			fAlphaFound=kTRUE;
			printf("    Alpha %8d Delta %7.3f Mev %6.2f\n",
					fFissionEvent->fiEventNumber-fStackEvent->fiEventNumber,
					(Float_t)fTimeDiff/1000000.,fStackEvent->ffStopXLhitV/1000.
					);
		}}
		if(fTimeDiff > (fParam->Fission1Tmax+fParam->AlphaTmax))break; // out of time window
		if(fAlphaFound&fStackEvent->fisEvr){
			printf("    Evr   %8d Delta %7.3f Mev %6.2f\n",
					fFissionEvent->fiEventNumber-fStackEvent->fiEventNumber,
					(Float_t)fTimeDiff/1000000.,fStackEvent->ffStopXHhitV/1000.
					);
		}
	}
	fEventStack->used=0;
	fEvent=(TascaEvent *) fEventStack->First();
	return;
}
fEvent->Copy(fInput); // copy event data into event entry
fEventStack->used++;
fEvent=(TascaEvent *) fEventStack->After(fEvent); // for next event
if(fEvent==0){ // no more slots
	if(!fStackfilled)
	cout<<"All "<<fEventStack->entries<<" slots used, start shuffling"<<endl;
	fStackfilled=kTRUE;
	fStackEvent=(TascaEvent *) fEventStack->First(); // the oldest one
	fEventStack->Remove(fStackEvent);  // remove from first
	fEventStack->AddLast(fStackEvent); // newest at the end
	fEvent=fStackEvent;
}
return;
} // BuildCalEvent
