//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "CompCheckEvent.h"
#include "CompEvent.h"

#include "Riostream.h"

#include "CompCheckProc.h"
#include "TGo4FileSource.h"

//***********************************************************
CompCheckEvent::CompCheckEvent()
  :TGo4EventElement(),fxCompCP(0),fxCompFS(0)
{
}
//***********************************************************
CompCheckEvent::CompCheckEvent(const char * name)
  :TGo4EventElement(name),fxCompCP(0),fxCompFS(0)
{
    cout << "Comp> CompCheckEvent "<<name<<" created"<< endl;
}
//***********************************************************
CompCheckEvent::~CompCheckEvent()
{
    //cout << "Comp> CompCheckEvent: Deleted"<< endl;
}
//***********************************************************


//-----------------------------------------------------------
Int_t CompCheckEvent::Init()
{
   // check for different source types
  Int_t rev = 0;
  if(CheckEventSource("CompCheckProc")){
    fxCompCP = (CompCheckProc*)GetEventSource();
    cout << "Comp> CompCheckEvent init for Check step"<< endl;
  }
  // or is it used from Analysis step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxCompFS = (TGo4FileSource*)GetEventSource();
    cout << "Comp> CompCheckEvent init for Analysis step"<< endl;
  }
   else     rev=1;
   return rev;

}
//-----------------------------------------------------------
Int_t CompCheckEvent::Fill()
{
   // check for different source types
   Int_t rev = 0;
   Clear();
   if(fxCompCP)fxCompCP->CompEventCheck(this);
   if(fxCompFS)fxCompFS->BuildEvent(this); // method from framework to restore event from file
   return rev;

}
//-----------------------------------------------------------
void CompCheckEvent::CopyTo(CompEvent *pp){
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
    pp->fiMultiStopXL=fiMultiStopXL;
    pp->fiMultiStopXH=fiMultiStopXL;
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
	pp->fiSystemmsec=fiSystemmsec;
	pp->fiSystemMysec=fiSystemMysec;
	pp->fiGammaMysec=fiGammaMysec;
  return;
}
//-----------------------------------------------------------
void CompCheckEvent::PrintEvent()
{
	printf("CheckEvent Run %3d File %4d Evt %9d Tof:%d Off:%d Veto:%d EVR:%d Al:%d SF:%d\n",
			fiFileNumber>>16,fiFileNumber&0xffff,fiEventNumber,
			fisTof,!fisMacro,fisVeto,fisEvr,fisAlpha,fisFission);
	printf("  [ms] %9u [mysec] Sys %9u Gam %9u Adc %9u, d [msec] Sys %6.3f Gam %6.3f Adc %6.3f\n",
			fiSystemmsec,fiSystemMysec,fiGammaMysec,fiTimeStamp,
			(Float_t)fiDeltaSystemTime/1000,
			(Float_t)fiDeltaGammaTime/1000,
			(Float_t)fiDeltaTime/1000);
	printf("  StopXL(H) i %3d (%3d), [MeV] %7.3f (%7.3f)",
			fiStopXLhitI,fiStopXHhitI,ffStopXLhitV/1000.,ffStopXHhitV/1000.);
	printf("  StopYL(H) i %3d (%3d), [MeV] %7.3f (%7.3f)\n",
			fiStopYLhitI,fiStopYHhitI,ffStopYLhitV/1000.,ffStopYHhitV/1000.);
	printf("  BackL(H)  i %3d (%3d), [MeV] %7.3f (%7.3f)",
			fiBackLhitI,fiBackHhitI,ffBackLhitV/1000.,ffBackHhitV/1000.);
	printf("  VetoL(H)  i %3d (%3d), [MeV] %7.3f (%7.3f)\n",
			fiVetoLhitI,fiVetoHhitI,ffVetoLhitV/1000.,ffVetoHhitV/1000.);
	printf("  Gamma Multi %d, [MeV] Sum %f Max %f   XMulti %2d (%2d)\n",
			fiMultiGamma,ffGammaSum/1000.,ffGammaMax/1000.,fiMultiStopXL,fiMultiStopXH);
}
