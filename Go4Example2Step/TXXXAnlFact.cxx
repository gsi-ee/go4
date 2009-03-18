#include "TXXXAnlFact.h"

#include "TXXXUnpackEvent.h"
#include "TXXXAnlEvent.h"
#include "TXXXAnlProc.h"

//***********************************************************
TXXXAnlFact::TXXXAnlFact()
: TGo4EventServerFactory()
{
}
//***********************************************************
TXXXAnlFact::TXXXAnlFact(const char* name)
: TGo4EventServerFactory(name)
{
}
//***********************************************************
TXXXAnlFact::~TXXXAnlFact()
{
}
//***********************************************************

TGo4EventProcessor * TXXXAnlFact::CreateEventProcessor(TGo4EventProcessorParameter* par)
{
   TXXXAnlProc*    anlproc      = new TXXXAnlProc("XXXAnlProc");
   return anlproc;
}
//-----------------------------------------------------------
TGo4EventElement * TXXXAnlFact::CreateOutputEvent()
{
   TXXXAnlEvent* anlevent = new TXXXAnlEvent("XXXAnlEvent");
   SetOutputEvent(anlevent); // important for some event store implementations!!!
   return anlevent;
}

//-----------------------------------------------------------
TGo4EventElement * TXXXAnlFact::CreateInputEvent()
{  // name is given in TXXXUnpackEvent
   TXXXUnpackEvent* event = new TXXXUnpackEvent();
   return event;
}
