//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

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
    cout << "Tasca> TascaCaliEvent "<<name<<" created"<< endl;
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
  // or is it used from Check step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxTascaFS = (TGo4FileSource*)GetEventSource();
    cout << "Tasca> TascaCaliEvent init for Check step"<< endl;
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
