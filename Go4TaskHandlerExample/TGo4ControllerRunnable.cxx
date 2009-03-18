#include "TGo4ControllerRunnable.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ExampleServer.h"
#include "TGo4ExampleController.h"

const UInt_t TGo4ControllerRunnable::fguCOMMANDINTERVAL=10000;

TGo4ControllerRunnable::TGo4ControllerRunnable(const char* name, TGo4ExampleServer* serv)
: TGo4Runnable(name,serv)
{
   TRACE((15,"TGo4ControllerRunnable::TGo4ControllerRunnable(Text_t*,TGo4ExampleServer*) constructor",__LINE__, __FILE__));
   fxController=serv->GetController();
   fxServer=serv;
}

TGo4ControllerRunnable::~TGo4ControllerRunnable()
{
   TRACE((15,"TGo4ControllerRunnable::~TGo4ControllerRunnable() destructor",__LINE__, __FILE__));
}

Int_t TGo4ControllerRunnable::Run(void*)
{
   TRACE((12,"TGo4ControllerRunnable::Run()",__LINE__, __FILE__));

   // we have to update the queues according to the current task handler
   TGo4TaskHandler* currenttask=fxServer->GetCurrentTaskHandler();
   if(currenttask==0)
      {
         TRACE((11,"TGo4ControllerRunnable::Run()-- no current client existing",__LINE__, __FILE__));
         TGo4Thread::Sleep(fguCOMMANDINTERVAL); // wait before sending next command
         // no client has connected to server so far, we do nothing!
      }
   else
      {
         TRACE((11,"TGo4ControllerRunnable::Run()-- working on client's taskhandler queues",__LINE__, __FILE__));
         // at least one client is there
         TGo4Thread::Sleep(fguCOMMANDINTERVAL); // wait before sending next command
         // send first command object created by controller:
         TGo4Command* com=fxController->GetCommand(); // command from controller instance, may poll here
         fxServer->SubmitCommand(com); // send controller command to client
         // send second command object by name:
         fxServer->SubmitCommand("EXAction2"); // send command by name
         TObject* data= fxServer->NextObject(); // get data from client
         fxController->DisplayData(data); // display data here on gui
      }
   return 0;

}
