#include "TascaCaliEvent.h"

#include "Riostream.h"

#include "TGo4FileSource.h"
#include "TascaCaliProc.h"


//***********************************************************
TascaCaliEvent::TascaCaliEvent() :
   TGo4EventElement(),fxTascaEP(0),fxTascaFS(0)
{
}
//***********************************************************
TascaCaliEvent::TascaCaliEvent(const char* name) :
   TGo4EventElement(name),fxTascaEP(0),fxTascaFS(0)
{
    cout << "Tasca> TascaCaliEvent: Create"<< endl;
}
//***********************************************************
TascaCaliEvent::~TascaCaliEvent()
{
    cout << "Tasca> TascaCaliEvent: Delete"<< endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t TascaCaliEvent::Init()
{
  Int_t rev=0;
  //cout << "+++ Init event" << endl;
  Clear();
  // is it used by Cali step as output?
  if(CheckEventSource("TascaCaliProc")){
    fxTascaEP = (TascaCaliProc*)GetEventSource();
    cout << "Tasca> TascaCaliEvent init for Cali step"<< endl;
  }
  // or is it used from Analysis step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxTascaFS = (TGo4FileSource*)GetEventSource();
    cout << "Tasca> TascaCaliEvent init for Analysis step"<< endl;
  }
  else          rev=1;
  return rev;
}
//-----------------------------------------------------------
Int_t TascaCaliEvent::Fill()
{
   Int_t rev=0;
   Clear();
   if(fxTascaEP)fxTascaEP->TascaCalibrate(this);  // user event processing method
   if(fxTascaFS)fxTascaFS->BuildEvent(this); // method from framework to restore event from file
   return rev;
}
