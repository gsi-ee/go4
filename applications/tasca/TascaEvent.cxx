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
	{
		printf("Event Chain %4d Run %3d File %4d Evt %9d Tof:%d Off:%d EVR:%d Al:%d SF:%d\n",
				fiChainNumber,fiFileNumber>>16,fiFileNumber&0xffff,fiEventNumber,
				fisTof,!fisMacro,fisEvr,fisAlpha,fisFission);
		printf("  [mysec] Sys %9u Gam %9u Adc %9u, d [msec] Sys %6.3f Gam %6.3f Adc %6.3f\n",
				fiSystemMysec,fiGammaMysec,fiTimeStamp,
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
		printf("  Gamma [MeV] Sum %f Max %f \n",ffGammaSum/1000.,ffGammaMax/1000.);
	}
  return 0;
}

