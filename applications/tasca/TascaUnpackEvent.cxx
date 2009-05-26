//---------------------------------------------
// Go4 Tasca analysis 
// Author: Hans G. Essel 
//         H.Essel@gsi.de 
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

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
  // or is it used from Cali step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxTascaFS = (TGo4FileSource*)GetEventSource();
    cout << "Tasca> TascaUnpackEvent init for Calibration step"<< endl;
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
