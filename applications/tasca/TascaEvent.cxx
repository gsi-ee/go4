//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "TascaEvent.h"

#include "Riostream.h"

//***********************************************************
TascaEvent::TascaEvent() {}
//***********************************************************
TascaEvent::TascaEvent(const char* name)
{
    cout << "Tasca> TascaEvent: " << name << " created" << endl;
}
//***********************************************************
TascaEvent::~TascaEvent(){
    cout << "Tasca> TascaEvent: " << GetName() << " deleted" << endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TascaEvent::PrintEvent(){
  return 0;
  cout << "Event " << GetName()<<":" <<endl;
  return 0;
}
//-----------------------------------------------------------
Bool_t TascaEvent::Copy(TascaCheckEvent *pp){
	fisTof=pp->fisTof;
	fisChopper=pp->fisChopper;
	fisMicro=pp->fisMicro;
	fisMacro=pp->fisMacro;
	fiEventNumber=pp->fiEventNumber;
	fiStopXLhitI=pp->fiStopXLhitI;
	fiStopXHhitI=pp->fiStopXHhitI;
	fiStopYLhitI=pp->fiStopYLhitI;
	fiStopYHhitI=pp->fiStopYHhitI;
	fiBackHhitI=pp->fiBackHhitI;
	fiBackLhitI=pp->fiBackLhitI;
	fiVetoHhitI=pp->fiVetoHhitI;
	fiVetoLhitI=pp->fiVetoLhitI;
	// value of maximum hit, if we had more than one hit
	ffStopXLhitV=pp->ffStopXLhitV;
	ffStopXHhitV=pp->ffStopXHhitV;
	ffStopYLhitV=pp->ffStopYLhitV;
	ffStopYHhitV=pp->ffStopYHhitV;
	ffBackHhitV =pp->ffBackHhitV;
	ffBackLhitV =pp->ffBackLhitV;
	ffVetoHhitV =pp->ffVetoHhitV;
	ffVetoLhitV =pp->ffVetoLhitV;
	fiTimeStamp=pp->fiTimeStamp;
	fiSystemMysec=pp->fiSystemMysec;
	fiDeltaTime=pp->fiDeltaTime;
	fiDeltaSystemTime=pp->fiDeltaSystemTime;
	fiGammaMysec = pp->fiGammaMysec;
	fiDeltaGammaMysec = pp->fiDeltaGammaMysec;
	for(Int_t i=0;i<7;i++){
		ffGammaKev[i] = pp->ffGammaKev[i];
		fiGammaChannelTime[i] = pp->fiGammaChannelTime[i];
	}

  return kTRUE;
}

