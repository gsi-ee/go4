#include "TYYYUnpackEvent.h"

#include "Riostream.h"

#include "TYYYUnpackProc.h"
#include "TGo4FileSource.h"

//***********************************************************
TYYYUnpackEvent::TYYYUnpackEvent() :
   TGo4EventElement("YYYUnpackEvent"),
   fxYYYEP(0),
   fxYYYFS(0)
{
}
//***********************************************************
TYYYUnpackEvent::TYYYUnpackEvent(const char* name) :
   TGo4EventElement(name),
   fxYYYEP(0),
   fxYYYFS(0)
{
}
//***********************************************************
TYYYUnpackEvent::~TYYYUnpackEvent()
{
}
//***********************************************************

//-----------------------------------------------------------
Int_t TYYYUnpackEvent::Init()
{
   Int_t rev=0;
   //cout << "+++ Init event" << endl;
   Clear();
   // is it used by Unpack step as output?
   if(CheckEventSource("TYYYUnpackProc")){
      fxYYYEP = (TYYYUnpackProc*)GetEventSource();
      cout << "**** YYYUnpackEvent init for Unpack step"<< endl;
   }
   // or is it used from Analysis step as input
   else if(CheckEventSource("TGo4FileSource")){
      fxYYYFS = (TGo4FileSource*)GetEventSource();
      cout << "**** YYYUnpackEvent init for Analysis step"<< endl;
   }
   else  rev=1;
   return rev;
}
//-----------------------------------------------------------
Int_t TYYYUnpackEvent::Fill()
{
   Int_t rev=0;
   Clear();
   if(fxYYYEP)fxYYYEP->YYYUnpack(this);  // user event processing method
   if(fxYYYFS)fxYYYFS->BuildEvent(this); // method from framework to restore event from file
   return rev;
}

//-----------------------------------------------------------
void  TYYYUnpackEvent::Clear(Option_t *t)
{
   void* destfield;
   //cout << "+++ event clear" << endl;
   destfield = (void*) &fdR[0];
   memset(destfield,0, sizeof(fdR));
   destfield = (void*) &fdV[0];
   memset(destfield,0, sizeof(fdV));
   destfield = (void*) &fdGam[0];
   memset(destfield,0, sizeof(fdGam));
   fiNumScatt=0;
   fdEmittX=0;
   fdEmittY=0;
   fdEmitt4d=0;

   //cout << "+++ event clear" << endl;
}
