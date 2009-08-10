//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#include "CompUnpackEvent.h"

#include "Riostream.h"

#include "TGo4FileSource.h"
#include "CompUnpackProc.h"


//***********************************************************
CompUnpackEvent::CompUnpackEvent() :
   TGo4EventElement(),fxCompEP(0),fxCompFS(0)
{
}
//***********************************************************
CompUnpackEvent::CompUnpackEvent(const char* name) :
   TGo4EventElement(name),fxCompEP(0),fxCompFS(0)
{
    cout << "Comp> CompUnpackEvent "<<name<<" created"<< endl;
}
//***********************************************************
CompUnpackEvent::~CompUnpackEvent()
{
    //cout << "Comp> CompUnpackEvent: Delete"<< endl;
}
//***********************************************************

//-----------------------------------------------------------
Int_t CompUnpackEvent::Init()
{
  Int_t rev=0;
  //cout << "+++ Init event" << endl;
  Clear();
  // is it used by Unpack step as output?
  if(CheckEventSource("CompUnpackProc")){
    fxCompEP = (CompUnpackProc*)GetEventSource();
    cout << "Comp> CompUnpackEvent init for Unpack step"<< endl;
  }
  // or is it used from Cali step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxCompFS = (TGo4FileSource*)GetEventSource();
    cout << "Comp> CompUnpackEvent init for Checker step"<< endl;
  }
  else          rev=1;
  return rev;
}
//-----------------------------------------------------------
Int_t CompUnpackEvent::Fill()
{
   Int_t rev=0;
   Clear();
   if(fxCompEP)fxCompEP->CompUnpack(this);  // user event processing method
   if(fxCompFS)fxCompFS->BuildEvent(this); // method from framework to restore event from file
   return rev;
}
