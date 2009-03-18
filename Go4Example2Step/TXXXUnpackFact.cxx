#include "TXXXUnpackFact.h"

#include "TXXXUnpackEvent.h"
#include "TXXXUnpackProc.h"

//***********************************************************
TXXXUnpackFact::TXXXUnpackFact()
: TGo4EventServerFactory()
{
}
//***********************************************************
TXXXUnpackFact::TXXXUnpackFact(const char* name)
: TGo4EventServerFactory(name)
{
}
//***********************************************************
TXXXUnpackFact::~TXXXUnpackFact()
{
}
//***********************************************************

//-----------------------------------------------------------
TGo4EventProcessor * TXXXUnpackFact::CreateEventProcessor(TGo4EventProcessorParameter* par)
{
   TXXXUnpackProc* proc = new TXXXUnpackProc("XXXUnpackProc");
   return proc;
}

//-----------------------------------------------------------
TGo4EventElement * TXXXUnpackFact::CreateOutputEvent()
{ // name is given in TXXXUnpackEvent
   TXXXUnpackEvent * event = new TXXXUnpackEvent();
   SetOutputEvent(event); // important for some event store implementations!!!
   return event;
}
