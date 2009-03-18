#include "TGo4WatchRunnable.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ClientStatus.h"

const UInt_t TGo4WatchRunnable::fguWATCHINTERVAL=5000;

TGo4WatchRunnable::TGo4WatchRunnable(const char* name, TGo4ExampleClient* cli)
: TGo4Runnable(name,cli)
{
   TRACE((15,"TGo4WatchRunnable::TGo4WatchRunnable(Text_t*,TGo4ExampleClient*) constructor",__LINE__, __FILE__));
   fxApplication=cli->GetApplication();
}

TGo4WatchRunnable::~TGo4WatchRunnable()
{
  TRACE((15,"TGo4WatchRunnable::~TGo4WatchRunnable() destructor",__LINE__, __FILE__));

}

Int_t TGo4WatchRunnable::Run(void*)
{
   TRACE((12,"TGo4WatchRunnable::Run()",__LINE__, __FILE__));
   TGo4ClientTask* cli= dynamic_cast<TGo4ClientTask*> (fxManager);
   if(cli)
   {
      cli->UpdateStatusBuffer(); // will create status object and stream it into TBuffer
      cli->SendStatusBuffer();   // send TBuffer with most recent client status
       // note: updating and sending of status buffer may be separated
       // into different threads. Updating aquires main mutex, sending usually
       // works without main mutex.
   if(fxApplication)
      {
         cli->SendStatusMessage(1,kTRUE,"Application %s of client %s has been watched",
                              fxApplication->GetName(),
                              cli->GetName());
         // method to pass text messages of priorities 0,1,2
         // (info, warning, error) to the server. kTRUE means additional local printout
      }
   }
   TGo4Thread::Sleep(fguWATCHINTERVAL); // wait for the next update
   return 0;
}
