#include "TGo4EventServerFactory.h"

#include "TGo4Log.h"

#include "TGo4MbsEvent.h"

#include "TGo4EventStore.h"
#include "TGo4FileStore.h"
#include "TGo4BackStore.h"
#include "TGo4TreeStore.h"
#include "TGo4FileStoreParameter.h"
#include "TGo4BackStoreParameter.h"
#include "TGo4TreeStoreParameter.h"

#include "TGo4EventSource.h"
#include "TGo4FileSource.h"
#include "TGo4TreeSource.h"
#include "TGo4MbsFile.h"
#include "TGo4MbsStream.h"
#include "TGo4MbsTransport.h"
#include "TGo4MbsEventServer.h"
#include "TGo4RevServ.h"
#include "TGo4MbsRandom.h"
#include "TGo4FileSourceParameter.h"
#include "TGo4TreeSourceParameter.h"
#include "TGo4MbsFileParameter.h"
#include "TGo4MbsStreamParameter.h"
#include "TGo4MbsTransportParameter.h"
#include "TGo4MbsEventServerParameter.h"
#include "TGo4RevServParameter.h"
#include "TGo4MbsRandomParameter.h"


TGo4EventServerFactory::TGo4EventServerFactory(const char* name)
: TGo4EventFactory(name), fxEvent(0)
{
   TRACE((15,"TGo4EventServerFactory::TGo4EventServerFactory(Text_t*)",__LINE__, __FILE__));
}

TGo4EventServerFactory::TGo4EventServerFactory()
: TGo4EventFactory("Go4 Event Server Factory"),fxEvent(0)
{
   TRACE((15,"TGo4EventServerFactory::TGo4EventServerFactory()",__LINE__, __FILE__));
}


TGo4EventServerFactory::~TGo4EventServerFactory()
{
   TRACE((15,"TGo4EventServerFactory::~TGo4EventServerFactory()",__LINE__, __FILE__));
}

TGo4EventElement * TGo4EventServerFactory::CreateInputEvent()
{
   TRACE((14,"TGo4EventServerFactory::CreateInputEvent()",__LINE__, __FILE__));

//    variant 1: create one default subevent with default identifiers
//   Short_t procidfield[1]= {1};
//   Char_t subcratefield[1]={0};
//   Char_t ctrlfield[1]={0};
//   UInt_t sizefield[1]={1};
//   TGo4MbsEvent* event = new TGo4MbsEvent(1, subcratefield, ctrlfield, procidfield, sizefield);
//   variant 2: empty event without predefined subevents
   TGo4MbsEvent* event = new TGo4MbsEvent(0, 0, 0, 0, 0);
   TGo4Log::Debug(" EventServer Factory: creating input event %s ",
            event->ClassName());
   return event;

}


TGo4EventStore * TGo4EventServerFactory::CreateEventStore(TGo4EventStoreParameter* par)
{
   TRACE((14,"TGo4EventServerFactory::CreateEventStore(TGo4EventStoreParameter*)",__LINE__, __FILE__));

   TGo4EventStore* rev=0;

   TGo4FileStore* fst=0; // tell Together we know these classes
   TGo4TreeStore* tst=0; // Together scans by pointer type...
   fst=0; // suppress compiler warnings
   tst=0;
   if(par)
      {
         if(!strcmp(par->ClassName(),"TGo4FileStoreParameter"))
            {
               rev= new TGo4FileStore(dynamic_cast<TGo4FileStoreParameter* > (par) );
               //par->PrintParameter();
            }
         else if(!strcmp(par->ClassName(),"TGo4TreeStoreParameter"))
            {
               //rev= new TGo4TreeStore(dynamic_cast<TGo4TreeStoreParameter* > (par), fxEvent );
               TGo4Log::Debug(" EventServer Factory: TreeStore is not recommended, PLEASE USE TGo4FileStore !!!! ");
               rev=0;
            }
         else if(!strcmp(par->ClassName(),"TGo4BackStoreParameter"))
            {
               rev= new TGo4BackStore(dynamic_cast<TGo4BackStoreParameter* > (par));
            }

         else
            {
               rev = 0;
            } // endif !strcmp(par->ClassName(),...)
       }
   else
      {
         rev = 0;
      }    // if(par)

   if(rev)
      {
         TGo4Log::Debug(" EventServer Factory: creating event store %s ",
               rev->ClassName());
      }
   else
      {
         TGo4Log::Debug(" EventServer Factory: Unknown eventstore parameter ");
      }
   return rev;




   return rev;

}


TGo4EventSource * TGo4EventServerFactory::CreateEventSource(TGo4EventSourceParameter * par)
{
   TRACE((14,"TGo4EventServerFactory::CreateEventSource(TGo4EventSourceParameter*)",__LINE__, __FILE__));


   TGo4EventSource* rev=0;

   if(par!=0)
      {
         if(!strcmp(par->ClassName(),"TGo4FileSourceParameter"))
            {
               rev= new TGo4FileSource(dynamic_cast<TGo4FileSourceParameter* > (par) );
            }
         else if(!strcmp(par->ClassName(),"TGo4TreeSourceParameter"))
            {
               //rev= new TGo4TreeSource(dynamic_cast<TGo4TreeSourceParameter* > (par) );
               TGo4Log::Debug(" EventServer Factory: TreeSource is not recommended, PLEASE USE TGo4FileSource !!!! ");
               rev=0;
            }
         else if(!strcmp(par->ClassName(),"TGo4MbsFileParameter"))
            {
               rev= new TGo4MbsFile(dynamic_cast<TGo4MbsFileParameter* > (par) );
               //par->PrintParameter();
            }
         else if(!strcmp(par->ClassName(),"TGo4MbsStreamParameter"))
            {
               rev= new TGo4MbsStream(dynamic_cast<TGo4MbsStreamParameter* > (par) );
            }
         else if(!strcmp(par->ClassName(),"TGo4MbsTransportParameter"))
            {
               rev= new TGo4MbsTransport(dynamic_cast<TGo4MbsTransportParameter* > (par) );
            }
         else if(!strcmp(par->ClassName(),"TGo4MbsEventServerParameter"))
            {
               rev= new TGo4MbsEventServer(dynamic_cast<TGo4MbsEventServerParameter* > (par) );
            }

         else if(!strcmp(par->ClassName(),"TGo4RevServParameter"))
            {
               rev= new TGo4RevServ(dynamic_cast<TGo4RevServParameter* > (par) );
            }
         else if(!strcmp(par->ClassName(),"TGo4MbsRandomParameter"))
            {
               rev= new TGo4MbsRandom(dynamic_cast<TGo4MbsRandomParameter* > (par) );
            }
         else if(!strcmp(par->ClassName(),"TGo4UserSourceParameter"))
            {
               rev= new TGo4MbsRandom(par->GetName());
            }
         else
            {
               rev = 0;
            }    // endif(!strcmp(par->ClassName(),...)
      }


   if(rev)
      {
         TGo4Log::Debug(" EventServer Factory: creating event source %s ",
               rev->ClassName());
      }
   else
      {
         TGo4Log::Debug(" EventServer Factory: Unknown eventsource parameter ");
      }
   return rev;

}



