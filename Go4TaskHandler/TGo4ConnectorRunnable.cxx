#include "TGo4ConnectorRunnable.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ServerTask.h"
#include "TGo4TaskManager.h"

TGo4ConnectorRunnable::TGo4ConnectorRunnable(const char* name, TGo4ServerTask* man)
   :TGo4Runnable(name,man)
{
   fxTaskManager=man->GetTaskManager();
}

TGo4ConnectorRunnable::~TGo4ConnectorRunnable()
{
}

Int_t TGo4ConnectorRunnable::Run(void* ptr)
{
   // create new taskhandler and connect server to  client:
   Int_t rev= fxTaskManager->ServeClient();
   if(rev< -1)
      {
         //cout <<"TGo4ConnectorRunnable::Run sees rev="<<rev << endl;
         fxGo4Thread->Stop(); // stop runnable for termination
      }
   else
      {
         // go on
      }
   return rev;
}
