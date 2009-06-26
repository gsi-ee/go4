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
TascaAnlProc::~TascaAnlProc()
{
	  cout << "Tasca> TascaAnlProc:    Processed "<<fiEventsProcessed<<" selected "<<fiEventsWritten << endl;
	  cout << "                  stack Processed "<<fiEvprocessedTotal<<endl;
	  cout << "                  total Fissions "<<fFissions <<" processed "<<fiSFprocessed<<endl;
	  cout << "                  total Alphas   "<<fAlphas << endl;
	  cout << "                  total EVRs     "<<fEvrs << endl;
if(fChainFile){
	fChainFile->Write();
	fChainFile->Close();
}
}
//***********************************************************
TascaAnlProc::TascaAnlProc()
  : TGo4EventProcessor(),fInput(0),fFissions(0),fAlphas(0),fEvrs(0)
{
}
//***********************************************************
// this one is used in TascaAnlFact
TascaAnlProc::TascaAnlProc(const char* name) :
  TGo4EventProcessor(name),fInput(0),fChainFile(0),fChainNumber(0),fChainStore(kFALSE),fFissions(0),fAlphas(0),fEvrs(0)
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

  // print description *********************************************************
  cout<<"*****************************************************************************"<<endl;
  cout<<"      Full stack, All fissions offbeam with Y, Y +-1 Time window fission2 plus alpha2"<<endl;
  cout<<"*****************************************************************************"<<endl;

  fEventStack=new TascaEventStack(fParam->EventStackSize);
  fStackIter = new TListIter(fEventStack);
  fEventIter = new TListIter(fEventStack);
  fEventCurrent=(TascaEvent *) fEventStack->First();
  fStackLinkCurrent=fEventStack->FirstLink();
  fStackfilled=kFALSE;
  fFirstEvent=0;
  fiEventsProcessed=0;
  fiEventsWritten=0;
  fiSFprocessed=0;
  fiSFtaken=0;
  fiEvprocessed=0;
  fiEvprocessedTotal=0;
  fAlphaFound=kFALSE;
  fEvrFound=kFALSE;
  fiLastFissionEvent=0;
  fChainStore=fControl->writeChainTree;
  }
//***********************************************************
void TascaAnlProc::PrintFission(Bool_t store){
printf(" --SF    %9d MevH: %6.2f  L: %6.2f BH: %6.2f L: %6.2f SF#:%6d X %3d Y %3d Spill %d\n",
		fFissionEvent->fiEventNumber,
		fFissionEvent->ffStopXHhitV/1000.,
		fFissionEvent->ffStopXLhitV/1000.,
		fFissionEvent->ffBackHhitV/1000.,
		fFissionEvent->ffBackLhitV/1000.,
		fFissions,
		fFissionEvent->fiStopXHhitI,
		fFissionEvent->fiStopYHhitI,
		fFissionEvent->fisMacro
);
fiEventsWritten++;
fFissionEvent->fiChainNumber=fChainNumber;
if(store){
fChainBranch->SetAddress(&fFissionEvent);
fChainTree->Fill();
}
return;
}
void TascaAnlProc::PrintAlpha(Bool_t store){
printf(" --Alpha %9d MevL: %6.2f BL: %6.2f toFission [s] %8.3f          X %3d Y %3d Spill %d\n",
		fStackEvent->fiEventNumber,
		fStackEvent->ffStopXLhitV/1000.,
		fStackEvent->ffBackLhitV/1000.,
		(Float_t)TimeDiff(fFissionEvent->fiTimeStamp,fStackEvent->fiTimeStamp)/1000000.,
		fStackEvent->fiStopXLhitI,
		fStackEvent->fiStopYLhitI,
		fStackEvent->fisMacro
);
fiEventsWritten++;
fStackEvent->fiChainNumber=fChainNumber;
if(store){
	fChainBranch->SetAddress(&fStackEvent);
	fChainTree->Fill();
	}
return;
}
void TascaAnlProc::PrintEvr(Bool_t store){
printf("Evr ---- %9d MevH: %6.2f            toFission [s] %8.3f          X %3d Y %3d Spill %d\n",
		fStackEvent->fiEventNumber,
		fStackEvent->ffStopXHhitV/1000.,
		(Float_t)TimeDiff(fFissionEvent->fiTimeStamp,fStackEvent->fiTimeStamp)/1000000.,
		fStackEvent->fiStopXHhitI,
		fStackEvent->fiStopYHhitI,
		fStackEvent->fisMacro
);
fiEventsWritten++;
fStackEvent->fiChainNumber=fChainNumber;
if(store){
	fChainBranch->SetAddress(&fStackEvent);
	fChainTree->Fill();
	}
return;
}
//-----------------------------------------------------------
void TascaAnlProc::TascaEventAnalysis(TascaAnlEvent* poutevt)
{
poutevt->SetValid(kFALSE);       // events are not stored until kTRUE is set
fInput  = (TascaCheckEvent*) GetInputEvent();
fiEventsProcessed++;
// Process only if event is valid
//cout <<"Anl: "<<fInput->fiEventNumber<< endl;
if(!fInput->IsValid()) return;
// Check if we should open output tree file
if(fChainStore){
if(fChainFile==0){
	  // open root file for output
	  strcpy(cfilename,fInput->GetEventSource()->GetActiveName());
	  char* pc=strstr(cfilename,"_Checked");
	  *pc=0;
	  strcat(cfilename,"_Chains.root");
	  cout<< "Tasca> TascaAnlProc "<<"create Chain file "<<cfilename<<endl;
	  fChainFile = new TFile(cfilename,"RECREATE");
	  // create tree
	  fChainTree = new TTree("SearcherTree","new");
	  fEvent=new TascaEvent();
	  // create one branch. The dot adds the event name to each member name
	  fChainBranch=fChainTree->Branch("Searched.","TascaEvent",&fEvent,100000,1);
}}

if(fInput->fisFission)fFissions++;
if(fInput->fisAlpha)fAlphas++;
if(fInput->fisEvr)fEvrs++;
if(fFirstEvent==0)fFirstEvent=fInput->fiEventNumber;

// We assume that fEventCurrent is a valid slot.
// With first event this is first slot
fInput->CopyTo(fEventCurrent); // copy event data into event entry
fEventStack->used++;
// From fission event we go back
 if(fInput->fisFission&&(!fInput->fisMacro)&&(fInput->fiStopYHhitI>=0)){
// if(fInput->fisFission&&(fInput->fiStopYHhitI>=0)){
// if(fInput->fisFission&(fInput->ffBackHhitV>10)){
// if(fInput->fisFission){
	fiSFprocessed++;
	fAlphaFound=kFALSE;
	fEvrFound=kFALSE;
	fFissionEvent=fEventCurrent; // keep that as end point
	fStackEvent=fEventCurrent;  // to step back
	fTimeDiff=fFissionEvent->fiDeltaTime;
	fStackLinkEntry=fStackLinkCurrent;
	// search backwards
	while((fStackLinkEntry=fStackLinkEntry->Prev())!=0){
		fStackEvent=(TascaEvent *) fStackLinkEntry->GetObject();
		//if(fTimeDiff <= fParam->Alpha2Tmax){ // in alpha time window
		  if(fStackEvent->fisAlpha
		     &&(fFissionEvent->fiStopXHhitI==fStackEvent->fiStopXLhitI)
		     &&((fFissionEvent->fiStopYHhitI==fStackEvent->fiStopYLhitI)
		       ||((fFissionEvent->fiStopYHhitI+1)==fStackEvent->fiStopYLhitI)
		       ||((fFissionEvent->fiStopYHhitI-1)==fStackEvent->fiStopYLhitI))
		     ){
			fAlphaFound=kTRUE;
		} //} // fission in time
		if(fStackEvent->fisEvr
		   &&(fFissionEvent->fiStopXHhitI==fStackEvent->fiStopXHhitI)
		   &&(fFissionEvent->fiStopYHhitI==fStackEvent->fiStopYHhitI)
		   ){
			fEvrFound=kTRUE;
			fiFileNumber=fStackEvent->fiFileNumber;
		}
		if(fTimeDiff > (fParam->Fission2Tmax+fParam->Alpha2Tmax))break; // out of time window
		fTimeDiff += fStackEvent->fiDeltaTime;
		fiEvprocessed++;
		fiEvprocessedTotal++;
	} // while back loop
	// We go now from here forward up to fission to print correct order
	fTimeDiff=0;
	if(fEvrFound&&fAlphaFound){
		fiSFtaken++;
		fChainNumber++;
		if(fStackLinkEntry==0){
			cout<<"End of stack, entries "<<fEventStack->used<<endl;
			fStackEvent=(TascaEvent *) fEventStack->First();
			fStackLinkEntry=fEventStack->FirstLink();
		}
		printf("Chain %4d Scope %7.3f sec Run %3d File %4d Stack %d --=========================================\n",
				fChainNumber,(Float_t)TimeDiff(fFissionEvent->fiTimeStamp,fStackEvent->fiTimeStamp)/1000000.,
				fiFileNumber>>16,fiFileNumber&0xffff,fiEvprocessed);
		fiEvprocessed=0;
		fAlphaFound=kFALSE;
		fEvrFound=kFALSE; //fake for offbeam
		while(fStackEvent!=fFissionEvent){
			if(fStackEvent->fisEvr
					&&(fFissionEvent->fiStopXHhitI==fStackEvent->fiStopXHhitI)
					&&(fFissionEvent->fiStopYHhitI==fStackEvent->fiStopYHhitI)
			){
				fEvrFound=kTRUE;
				PrintEvr(fChainStore);
			}
			else if(fStackEvent->fisAlpha
					&&(fFissionEvent->fiStopXHhitI==fStackEvent->fiStopXLhitI)
					&&((fFissionEvent->fiStopYHhitI==fStackEvent->fiStopYLhitI)
					||((fFissionEvent->fiStopYHhitI+1)==fStackEvent->fiStopYLhitI)
					||((fFissionEvent->fiStopYHhitI-1)==fStackEvent->fiStopYLhitI))
			){
				fAlphaFound=kTRUE;
				PrintAlpha(fChainStore);
			}
			else if((fStackEvent->fisFission)&&(fiLastFissionEvent==fStackEvent->fiEventNumber))
				printf("   Fission  %9d double\n",fStackEvent->fiEventNumber);
			fStackLinkEntry=fStackLinkEntry->Next();
			fStackEvent=(TascaEvent *) fStackLinkEntry->GetObject();
		} // second while loop forward
	} // found Alpha or Evr
	if(fEvrFound&&fAlphaFound){
		PrintFission(fChainStore);
		fiLastFissionEvent=fFissionEvent->fiEventNumber;
	}
} // fission
// Note that we have already filled the list with preallocated events.
// Therefore the iterator runs over all entries.
 // With return fEventCurrent must point to the next free item.
fStackLinkCurrent=fStackLinkCurrent->Next();
if(fStackLinkCurrent==0){
	if(!fStackfilled)
	cout<<"Run "<<(fInput->fiFileNumber>>16)
	    <<" file "<<(fInput->fiFileNumber&0xffff)
	    <<" all "<<fEventStack->entries<<" slots used, start shuffling"<<endl;
	fStackfilled=kTRUE;
	fStackEvent=(TascaEvent *) fEventStack->First(); // the oldest one
	fEventStack->Remove(fStackEvent);  // remove from first
	fEventStack->AddLast(fStackEvent); // newest at the end
	fEventCurrent=fStackEvent;
	fStackLinkCurrent=fEventStack->LastLink();

} else
	fEventCurrent=(TascaEvent *) fStackLinkCurrent->GetObject();
return;
} // BuildCalEvent
