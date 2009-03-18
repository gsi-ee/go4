#include "TXXXEvent.h"

#include "Riostream.h"

//***********************************************************
TXXXEvent::TXXXEvent() :
   TGo4EventElement()
{
    cout << "**** TXXXEvent: Create instance" << endl;
}
//***********************************************************
TXXXEvent::TXXXEvent(const char* name) :
   TGo4EventElement(name)
{
  cout << "**** TXXXEvent: Create instance " << name << endl;
}
//***********************************************************
TXXXEvent::~TXXXEvent()
{
  cout << "**** TXXXEvent: Delete instance " << endl;
}

//-----------------------------------------------------------
void  TXXXEvent::Clear(Option_t *t)
{
  // all members should be cleared.
   memset((void*) &fCrate1[0],0, sizeof(fCrate1));
   memset((void*) &fCrate2[0],0, sizeof(fCrate2));
}
