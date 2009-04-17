#include "TascaUnpackEvent.h"

#include "Riostream.h"

#include "TGo4FileSource.h"
#include "TascaUnpackProc.h"


//***********************************************************
TascaUnpackEvent::TascaUnpackEvent() :
   TGo4EventElement(),fxTascaEP(0),fxTascaFS(0)
{
}
//***********************************************************
TascaUnpackEvent::TascaUnpackEvent(const char* name) :
   TGo4EventElement(name),fxTascaEP(0),fxTascaFS(0)
{
    cout << "Tasca> TascaUnpackEvent: Create"<< endl;
}
//***********************************************************
TascaUnpackEvent::~TascaUnpackEvent()
{
    cout << "Tasca> TascaUnpackEvent: Delete"<< endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TascaUnpackEvent::Init()
{
  Int_t rev=0;
  //cout << "+++ Init event" << endl;
  Clear();
  // is it used by Unpack step as output?
  if(CheckEventSource("TascaUnpackProc")){
    fxTascaEP = (TascaUnpackProc*)GetEventSource();
    cout << "Tasca> TascaUnpackEvent init for Unpack step"<< endl;
  }
  // or is it used from Analysis step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxTascaFS = (TGo4FileSource*)GetEventSource();
    cout << "Tasca> TascaUnpackEvent init for Analysis step"<< endl;
  }
  else          rev=1;
  return rev;
}
//-----------------------------------------------------------
Int_t TascaUnpackEvent::Fill()
{
   Int_t rev=0;
   Clear();
   if(fxTascaEP)fxTascaEP->TascaUnpack(this);  // user event processing method
   if(fxTascaFS)fxTascaFS->BuildEvent(this); // method from framework to restore event from file
   return rev;
}

//-----------------------------------------------------------
void  TascaUnpackEvent::Clear(Option_t *t)
{
	   memset((void*) &fiAdc[0],  0, sizeof(fiAdc));
	   memset((void*) &fiStopXL[0],0, sizeof(fiStopXL));
	   memset((void*) &fiStopXH[0],0, sizeof(fiStopXH));
	   memset((void*) &fiStopYL[0],0, sizeof(fiStopYL));
	   memset((void*) &fiStopYH[0],0, sizeof(fiStopYH));
	   memset((void*) &fiBackL[0], 0, sizeof(fiBackL));
	   memset((void*) &fiBackH[0], 0, sizeof(fiBackH));
	   memset((void*) &fiVetoL[0], 0, sizeof(fiVetoL));
	   memset((void*) &fiVetoH[0], 0, sizeof(fiVetoH));
   //cout << "+++ event clear" << endl;
}
