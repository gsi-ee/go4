#include "TXXXUnpackEvent.h"

#include "Riostream.h"

#include "TGo4FileSource.h"
#include "TXXXUnpackProc.h"


//***********************************************************
TXXXUnpackEvent::TXXXUnpackEvent() :
   TGo4EventElement(),fxXXXEP(0),fxXXXFS(0)
{
}
//***********************************************************
TXXXUnpackEvent::TXXXUnpackEvent(const char* name) :
   TGo4EventElement(name),fxXXXEP(0),fxXXXFS(0)
{
}
//***********************************************************
TXXXUnpackEvent::~TXXXUnpackEvent()
{
}
//***********************************************************

//-----------------------------------------------------------
Int_t TXXXUnpackEvent::Init()
{
  Int_t rev=0;
  //cout << "+++ Init event" << endl;
  Clear();
  // is it used by Unpack step as output?
  if(CheckEventSource("TXXXUnpackProc")){
    fxXXXEP = (TXXXUnpackProc*)GetEventSource();
    cout << "**** XXXUnpackEvent init for Unpack step"<< endl;
  }
  // or is it used from Analysis step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxXXXFS = (TGo4FileSource*)GetEventSource();
    cout << "**** XXXUnpackEvent init for Analysis step"<< endl;
  }
  else          rev=1;
  return rev;
}
//-----------------------------------------------------------
Int_t TXXXUnpackEvent::Fill()
{
   Int_t rev=0;
   Clear();
   if(fxXXXEP)fxXXXEP->XXXUnpack(this);  // user event processing method
   if(fxXXXFS)fxXXXFS->BuildEvent(this); // method from framework to restore event from file
   return rev;
}

//-----------------------------------------------------------
void  TXXXUnpackEvent::Clear(Option_t *t)
{
   void* destfield;
   //cout << "+++ event clear" << endl;
   destfield = (void*) &fiCrate1[0];
   memset(destfield,0, sizeof(fiCrate1));
   destfield = (void*) &fiCrate2[0];
   memset(destfield,0, sizeof(fiCrate2));
   destfield = (void*) &fiCrate3[0];
   memset(destfield,0, sizeof(fiCrate3));
   destfield = (void*) &fiCrate4[0];
   memset(destfield,0, sizeof(fiCrate4));
   //cout << "+++ event clear" << endl;
}
