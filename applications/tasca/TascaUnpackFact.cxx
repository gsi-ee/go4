#include "TascaUnpackFact.h"

#include "TascaUnpackEvent.h"
#include "TascaUnpackProc.h"

//***********************************************************
TascaUnpackFact::TascaUnpackFact()
: TGo4EventServerFactory()
{
}
//***********************************************************
TascaUnpackFact::TascaUnpackFact(const char* name)
: TGo4EventServerFactory(name)
{
}
//***********************************************************
TascaUnpackFact::~TascaUnpackFact()
{
}
//***********************************************************

//-----------------------------------------------------------
TGo4EventProcessor * TascaUnpackFact::CreateEventProcessor(TGo4EventProcessorParameter* par)
{
   TascaUnpackProc* proc = new TascaUnpackProc("TascaUnpackProc");
   return proc;
}

//-----------------------------------------------------------
TGo4EventElement * TascaUnpackFact::CreateOutputEvent()
{ // name is given in TascaUnpackEvent
   TascaUnpackEvent * event = new TascaUnpackEvent();
   SetOutputEvent(event); // important for some event store implementations!!!
   return event;
}
