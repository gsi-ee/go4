#include "TascaAnlFact.h"

#include "TascaUnpackEvent.h"
#include "TascaAnlEvent.h"
#include "TascaAnlProc.h"

//***********************************************************
TascaAnlFact::TascaAnlFact()
: TGo4EventServerFactory()
{
}
//***********************************************************
TascaAnlFact::TascaAnlFact(const char* name)
: TGo4EventServerFactory(name)
{
}
//***********************************************************
TascaAnlFact::~TascaAnlFact()
{
}
//***********************************************************

TGo4EventProcessor * TascaAnlFact::CreateEventProcessor(TGo4EventProcessorParameter* par)
{
   TascaAnlProc*    anlproc      = new TascaAnlProc("TascaAnlProc");
   return anlproc;
}
//-----------------------------------------------------------
TGo4EventElement * TascaAnlFact::CreateOutputEvent()
{
   TascaAnlEvent* anlevent = new TascaAnlEvent("TascaAnlEvent");
   SetOutputEvent(anlevent); // important for some event store implementations!!!
   return anlevent;
}

//-----------------------------------------------------------
TGo4EventElement * TascaAnlFact::CreateInputEvent()
{  // name is given in TascaUnpackEvent
   TascaUnpackEvent* event = new TascaUnpackEvent();
   return event;
}
