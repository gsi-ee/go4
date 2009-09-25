#include "TGo4LoggingRunnable.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ExampleServer.h"
#include "TGo4ExampleController.h"

TGo4LoggingRunnable::TGo4LoggingRunnable(const char* name, TGo4ExampleServer* serv)
: TGo4Runnable(name,serv)
{
   TRACE((15,"TGo4LoggingRunnable::TGo4LoggingRunnable(const char*,TGo4ExampleServer*) constructor",__LINE__, __FILE__));
   fxController=serv->GetController();
   fxServer=serv;
}

TGo4LoggingRunnable::~TGo4LoggingRunnable()
{
   TRACE((15,"TGo4LoggingRunnable::TGo4LoggingRunnable() constructor",__LINE__, __FILE__));
}

Int_t TGo4LoggingRunnable::Run(void*)
{
   TRACE((12,"TGo4LoggingRunnable::Run()",__LINE__, __FILE__));
   TGo4TaskHandler* currenttask=fxServer->GetCurrentTaskHandler();
   if(currenttask==0) {
      TRACE((11,"TGo4LoggingRunnable::Run()-- no current client existing",__LINE__, __FILE__));
      TGo4Thread::Sleep(5000);
      // no client has connected to server so far, we do nothing!
   } else {
      TRACE((11,"TGo4LoggingRunnable::Run()-- working on client's taskhandler queues",__LINE__, __FILE__));
      TGo4Status* status= fxServer->NextStatus();
      fxController->DisplayLog(status);
   }
   return 0;
}
