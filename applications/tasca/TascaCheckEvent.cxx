//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaCheckEvent.h"
#include "TascaEvent.h"

#include "Riostream.h"

#include "TascaCheckProc.h"
#include "TGo4FileSource.h"

//***********************************************************
TascaCheckEvent::TascaCheckEvent()
  :TGo4EventElement(),fxTascaCP(0),fxTascaFS(0)
{
}
//***********************************************************
TascaCheckEvent::TascaCheckEvent(const char * name)
  :TGo4EventElement(name),fxTascaCP(0),fxTascaFS(0)
{
    cout << "Tasca> TascaCheckEvent "<<name<<" created"<< endl;
}
//***********************************************************
TascaCheckEvent::~TascaCheckEvent()
{
    cout << "Tasca> TascaCheckEvent: Deleted"<< endl;
}
//***********************************************************


//-----------------------------------------------------------
Int_t TascaCheckEvent::Init()
{
   // check for different source types
  Int_t rev = 0;
  if(CheckEventSource("TascaCheckProc")){
    fxTascaCP = (TascaCheckProc*)GetEventSource();
    cout << "Tasca> TascaCheckEvent init for Check step"<< endl;
  }
  // or is it used from Analysis step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxTascaFS = (TGo4FileSource*)GetEventSource();
    cout << "Tasca> TascaCheckEvent init for Analysis step"<< endl;
  }
   else     rev=1;
   return rev;

}
//-----------------------------------------------------------
Int_t TascaCheckEvent::Fill()
{
   // check for different source types
   Int_t rev = 0;
   Clear();
   if(fxTascaCP)fxTascaCP->TascaEventCheck(this);
   if(fxTascaFS)fxTascaFS->BuildEvent(this); // method from framework to restore event from file
   return rev;

}
//-----------------------------------------------------------
void TascaCheckEvent::CopyTo(TascaEvent *pp){
	pp->fisTof=fisTof;
	pp->fisChopper=fisChopper;
	pp->fisMicro=fisMicro;
	pp->fisMacro=fisMacro;
	pp->fisEvr=fisEvr;
	pp->fisAlpha=fisAlpha;
	pp->fisFission=fisFission;
	pp->fiFileNumber=fiFileNumber;
	pp->fiEventNumber=fiEventNumber;

	pp->fiStopXLhitI=fiStopXLhitI;
	pp->fiStopXHhitI=fiStopXHhitI;
	pp->fiStopYLhitI=fiStopYLhitI;
	pp->fiStopYHhitI=fiStopYHhitI;
	pp->fiBackHhitI=fiBackHhitI;
	pp->fiBackLhitI=fiBackLhitI;
	pp->fiVetoHhitI=fiVetoHhitI;
	pp->fiVetoLhitI=fiVetoLhitI;
	// value of maximum hit, if we had more than one hit
	pp->ffStopXLhitV=ffStopXLhitV;
	pp->ffStopXHhitV=ffStopXHhitV;
	pp->ffStopYLhitV=ffStopYLhitV;
	pp->ffStopYHhitV=ffStopYHhitV;
	pp->ffBackHhitV =ffBackHhitV;
	pp->ffBackLhitV =ffBackLhitV;
	pp->ffVetoHhitV =ffVetoHhitV;
	pp->ffVetoLhitV =ffVetoLhitV;
	pp->fiDeltaSystemTime=fiDeltaSystemTime;
	pp->fiDeltaGammaTime=fiDeltaGammaTime;
	pp->fiDeltaTime=fiDeltaTime;
	pp->ffGammaMax=ffGammaMax;
	pp->ffGammaSum=ffGammaSum;

	pp->fiTimeStamp=fiTimeStamp;
	pp->fiSystemMysec=fiSystemMysec;
	pp->fiGammaMysec=fiGammaMysec;
  return;
}
