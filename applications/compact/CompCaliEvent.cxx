//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "CompCaliEvent.h"

#include "Riostream.h"

#include "TGo4FileSource.h"
#include "CompCaliProc.h"


//***********************************************************
CompCaliEvent::CompCaliEvent() :
   TGo4EventElement(),fxCompEP(0),fxCompFS(0)
{
}
//***********************************************************
CompCaliEvent::CompCaliEvent(const char* name) :
   TGo4EventElement(name),fxCompEP(0),fxCompFS(0)
{
    cout << "Comp> CompCaliEvent "<<name<<" created"<< endl;
}
//***********************************************************
CompCaliEvent::~CompCaliEvent()
{
    //cout << "Comp> CompCaliEvent: Delete"<< endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t CompCaliEvent::Init()
{
  Int_t rev=0;
  //cout << "+++ Init event" << endl;
  Clear();
  // is it used by Cali step as output?
  if(CheckEventSource("CompCaliProc")){
    fxCompEP = (CompCaliProc*)GetEventSource();
    cout << "Comp> CompCaliEvent init for Cali step"<< endl;
  }
  // or is it used from Check step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxCompFS = (TGo4FileSource*)GetEventSource();
    cout << "Comp> CompCaliEvent init for Check step"<< endl;
  }
  else          rev=1;
  return rev;
}
//-----------------------------------------------------------
Int_t CompCaliEvent::Fill()
{
   Int_t rev=0;
   Clear();
   if(fxCompEP)fxCompEP->CompCalibrate(this);  // user event processing method
   if(fxCompFS)fxCompFS->BuildEvent(this); // method from framework to restore event from file
   return rev;
}
//-----------------------------------------------------------
void CompCaliEvent::PrintEvent()
{
	printf("CaliEvent Run %3d File %4d Evt %9d Tof:%d Off:%d Veto:%d\n",
			fiFileNumber>>16,fiFileNumber&0xffff,fiEventNumber,
			fisTof,!fisMacro,fisVeto);
	printf("  [ms] %9u [mysec] Sys %9u Gam %9u Adc %9u\n",
			fiSystemmsec,fiSystemMysec,fiGammaMysec,fiTimeStamp);
	printf("  Multi StopXL %d StopXH %d StopYL %d StopYH %d BackL %d BackH %d VetoL %d VetoH %d\n",
			fiMultiStopXL,fiMultiStopXH,fiMultiStopYL,fiMultiStopYH,
			fiMultiBackL,fiMultiBackH,fiMultiVetoL,fiMultiVetoH);
	printf("  StopXL(H) i %3d (%3d), [MeV] %7.3f (%7.3f)",
			fiStopXLhitI,fiStopXHhitI,ffStopXLhitV/1000.,ffStopXHhitV/1000.);
	printf("  StopYL(H) i %3d (%3d), [MeV] %7.3f (%7.3f)\n",
			fiStopYLhitI,fiStopYHhitI,ffStopYLhitV/1000.,ffStopYHhitV/1000.);
	printf("  BackL(H)  i %3d (%3d), [MeV] %7.3f (%7.3f)",
			fiBackLhitI,fiBackHhitI,ffBackLhitV/1000.,ffBackHhitV/1000.);
	printf("  VetoL(H)  i %3d (%3d), [MeV] %7.3f (%7.3f)\n",
			fiVetoLhitI,fiVetoHhitI,ffVetoLhitV/1000.,ffVetoHhitV/1000.);
	printf("  Gamma Multi %d, [MeV] Sum %f Max %f \n",fiMultiGamma,ffGammaSum/1000.,ffGammaMax/1000.);
}
