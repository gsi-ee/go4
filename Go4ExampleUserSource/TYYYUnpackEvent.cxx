#include "TYYYUnpackEvent.h"

//***********************************************************
TYYYUnpackEvent::TYYYUnpackEvent() :
   TGo4EventElement()
{
}
//***********************************************************
TYYYUnpackEvent::TYYYUnpackEvent(const char* name) :
   TGo4EventElement(name)
{
}
//***********************************************************
TYYYUnpackEvent::~TYYYUnpackEvent()
{
}
//***********************************************************

//-----------------------------------------------------------
void  TYYYUnpackEvent::Clear(Option_t *t)
{
   void* destfield;
   //cout << "+++ event clear" << endl;
   destfield = (void*) &fdR[0];
   memset(destfield, 0, sizeof(fdR));
   destfield = (void*) &fdV[0];
   memset(destfield, 0, sizeof(fdV));
   destfield = (void*) &fdGam[0];
   memset(destfield, 0, sizeof(fdGam));
   fiNumScatt=0;
   fdEmittX=0;
   fdEmittY=0;
   fdEmitt4d=0;
}
