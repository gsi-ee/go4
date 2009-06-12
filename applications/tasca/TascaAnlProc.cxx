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
  fEvrFound=kFALSE;
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
void TascaAnlProc::PrintFission(Bool_t store){
printf("Fission %3d: %9d  MevH:%6.2f L: %6.2f Back H: %6.2f L: %6.2f X %3d Y %3d\n",
		fFissions,
		fFissionEvent->fiEventNumber,
		fFissionEvent->ffStopXHhitV/1000.,
		fFissionEvent->ffStopXLhitV/1000.,
		fFissionEvent->ffBackHhitV/1000.,
		fFissionEvent->ffBackLhitV/1000.,
		fFissionEvent->fiStopXHhitI,
		fFissionEvent->fiStopYHhitI
);
return;
}
void TascaAnlProc::PrintAlpha(Bool_t store){
printf("    Alpha %8d MevL: %6.2f toFission [s] %7.3f                  X %3d Y %3d\n",
		fFissionEvent->fiEventNumber-fStackEvent->fiEventNumber,
		fStackEvent->ffStopXLhitV/1000.,
		(Float_t)TimeDiff(fFissionEvent->fiTimeStamp,fStackEvent->fiTimeStamp)/1000000.,
		fStackEvent->fiStopXLhitI,
		fStackEvent->fiStopYLhitI
);
return;
}
void TascaAnlProc::PrintEvr(Bool_t store){
printf("    Evr   %8d MevH: %6.2f toFission [s] %7.3f                  X %3d Y %3d\n",
		fFissionEvent->fiEventNumber-fStackEvent->fiEventNumber,
		fStackEvent->ffStopXHhitV/1000.,
		(Float_t)TimeDiff(fFissionEvent->fiTimeStamp,fStackEvent->fiTimeStamp)/1000000.,
		fStackEvent->fiStopXHhitI,
		fStackEvent->fiStopYHhitI
);
return;
}
//-----------------------------------------------------------
void TascaAnlProc::TascaEventAnalysis(TascaAnlEvent* poutevt)
{
poutevt->SetValid(kFALSE);       // events are not stored until kTRUE is set
fInput  = (TascaCheckEvent*) GetInputEvent();
// Process only if event is valid
//cout <<"Anl: "<<fInput->fiEventNumber<< endl;
if(!fInput->IsValid()) return;

if(fInput->fisFission)fFissions++;
if(fInput->fisAlpha)fAlphas++;
if(fInput->fisEvr)fEvrs++;
if(fFirstEvent==0)fFirstEvent=fInput->fiEventNumber;

// We assume that fEvent is a valid slot.
// With fisrt event this is first slot
// From fission event we go back
if(fInput->fisFission){
	fAlphaFound=kFALSE;
	fEvrFound=kFALSE;
	fInput->CopyTo(fEvent); // copy event data into event entry
	fEventStack->used++;
	fFissionEvent=fEvent; // keep that as end point
	fStackEvent=fEvent;  // to step back
	fTimeDiff=fFissionEvent->fiDeltaTime;
    	//printf("=============================\n");
	//PrintFission(kFALSE);
	while((fStackEvent=(TascaEvent *) fEventStack->Before(fStackEvent))!=0){
		if(fTimeDiff <= fParam->Fission1Tmax){ // in alpha time window
		  if(fStackEvent->fisAlpha
		     &(fFissionEvent->fiStopXHhitI==fStackEvent->fiStopXLhitI)
		     &(fFissionEvent->fiStopYHhitI==fStackEvent->fiStopYLhitI)
		     ){
			//PrintAlpha(kFALSE);
			fAlphaFound=kTRUE;
		}} // fission in time
		if(fTimeDiff > (fParam->Fission1Tmax+fParam->AlphaTmax))break; // out of time window
		if(fStackEvent->fisEvr
		   &(fFissionEvent->fiStopXHhitI==fStackEvent->fiStopXHhitI)
		   &(fFissionEvent->fiStopYHhitI==fStackEvent->fiStopYHhitI)
		   ){
			fEvrFound=kTRUE;
			//PrintEvr(kFALSE);
		}
		fTimeDiff += fStackEvent->fiDeltaTime;
	} // while back loop
	//' We go now from here forward up to fission to print correct order
	fTimeDiff=0;
	if((fEvrFound|fAlphaFound)){
	if(fStackEvent==0)fStackEvent=(TascaEvent *) fEventStack->First();
    	printf("%7.3f sec =============================\n",
		(Float_t)TimeDiff(fFissionEvent->fiTimeStamp,fStackEvent->fiTimeStamp)/1000000.);
	fAlphaFound=kFALSE;
	fEvrFound=kFALSE;
	while(fStackEvent!=fFissionEvent){
		if(fStackEvent->fisEvr
		   &(fFissionEvent->fiStopXHhitI==fStackEvent->fiStopXHhitI)
		   &(fFissionEvent->fiStopYHhitI==fStackEvent->fiStopYHhitI)
		   ){
			fEvrFound=kTRUE;
			PrintEvr(kFALSE);
		}
		if(fStackEvent->fisAlpha
		     &(fFissionEvent->fiStopXHhitI==fStackEvent->fiStopXLhitI)
		     &(fFissionEvent->fiStopYHhitI==fStackEvent->fiStopYLhitI)
		   ){
			fAlphaFound=kTRUE;
			PrintAlpha(kFALSE);
		} // fission in time
		fStackEvent=(TascaEvent *) fEventStack->After(fStackEvent);
	} // second while loop forward
	} // found Alpha or Evr
	if(fEvrFound|fAlphaFound) PrintFission(kFALSE);
	fEventStack->used=0;
	fEvent=(TascaEvent *) fEventStack->First();
	return;
} // fission
// Note that we have already filled the list with preallocated events.
// Therefore the iterator runs over all entries.
fInput->CopyTo(fEvent); // copy event data into event entry
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
