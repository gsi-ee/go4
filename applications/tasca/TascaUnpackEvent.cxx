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
}
//***********************************************************
TascaUnpackEvent::~TascaUnpackEvent()
{
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
    cout << "**** TascaUnpackEvent init for Unpack step"<< endl;
  }
  // or is it used from Analysis step as input
  else if(CheckEventSource("TGo4FileSource")){
    fxTascaFS = (TGo4FileSource*)GetEventSource();
    cout << "**** TascaUnpackEvent init for Analysis step"<< endl;
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
