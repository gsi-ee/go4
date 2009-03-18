#include "TYYYUnpackFact.h"

#include "Riostream.h"

#include "TGo4EventSourceParameter.h"
#include "TGo4UserSourceParameter.h"

#include "TYYYRawEvent.h"
#include "TYYYUnpackEvent.h"
#include "TYYYUnpackProc.h"
#include "TYYYEventSource.h"

//***********************************************************
TYYYUnpackFact::TYYYUnpackFact(const char* name)
: TGo4EventServerFactory(name)
{
}
//***********************************************************
TYYYUnpackFact::TYYYUnpackFact()
: TGo4EventServerFactory("YYY Event Fact")
{
}
//***********************************************************
TYYYUnpackFact::~TYYYUnpackFact()
{
}
//***********************************************************

TGo4EventElement * TYYYUnpackFact::CreateInputEvent()
{
   TYYYRawEvent* event = new TYYYRawEvent("YYYRawEvent");
   return event;

}

//-----------------------------------------------------------
TGo4EventProcessor * TYYYUnpackFact::CreateEventProcessor(TGo4EventProcessorParameter* par)
{
   TYYYUnpackProc* proc = new TYYYUnpackProc("YYYUnpackProc");
   return proc;
}

//-----------------------------------------------------------
TGo4EventElement * TYYYUnpackFact::CreateOutputEvent()
{ // name is given in TYYYUnpackEvent
   TYYYUnpackEvent * event = new TYYYUnpackEvent();
   SetOutputEvent(event); // important for some event store implementations!!!
   return event;
}

//-----------------------------------------------------------
TGo4EventSource * TYYYUnpackFact::CreateEventSource(TGo4EventSourceParameter * par)
{
TGo4EventSource* rev=0;
if(par)
      {
         if(!strcmp(par->ClassName(),"TGo4UserSourceParameter"))
            {
               rev= new TYYYEventSource(dynamic_cast<TGo4UserSourceParameter* > (par) );
               cout <<"TYYYUnpackFact::CreateEventSource with UserSourceParameter" << endl;
            }
         else
            {
               rev=TGo4EventServerFactory::CreateEventSource(par);
               cout <<"TYYYUnpackFact::CreateEventSource with parameter "<<par->GetName() << endl;

            }
      }
return rev;
}
