#include "TXXXAnlEvent.h"

//***********************************************************
TXXXAnlEvent::TXXXAnlEvent() : TGo4EventElement()
{
}
//***********************************************************
TXXXAnlEvent::TXXXAnlEvent(const char * name) : TGo4EventElement(name)
{
}
//***********************************************************
TXXXAnlEvent::~TXXXAnlEvent()
{
}

//-----------------------------------------------------------
void TXXXAnlEvent::Clear(Option_t *t)
{
  void* destfield;
  destfield = (void*) &frData[0];
  memset(destfield,0, sizeof(frData));
}
