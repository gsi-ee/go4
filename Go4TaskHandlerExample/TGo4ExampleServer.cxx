#include "TGo4ExampleServer.h"

#include "Riostream.h"

#include "snprintf.h"

#include "TGo4Log.h"
#include "TGo4Status.h"
#include "TGo4BufferQueue.h"
#include "TGo4ThreadHandler.h"

#include "TGo4LoggingRunnable.h"
#include "TGo4ControllerRunnable.h"
#include "TGo4ExampleCommandList.h"

const Text_t TGo4ExampleServer::fgcLOGGINGTHREAD[]="LOGGER-";
const Text_t TGo4ExampleServer::fgcCONTROLTHREAD[]="CONTROL-";
TGo4ExampleController* TGo4ExampleServer::GetController()
{
   TRACE((12,"TGo4ExampleServer::GetController()",__LINE__, __FILE__));

   return fxController;
}
TGo4ExampleServer::~TGo4ExampleServer()
{
   TRACE((15,"TGo4ExampleServer::~TGo4ExampleServer() destructor",__LINE__, __FILE__));

   delete [] fcControlName;
   delete [] fcLoggingName;
   fxWorkHandler->CancelAll(); // make sure threads wont work on controller instance when its deleted
   delete fxController;

}
TGo4ExampleServer::TGo4ExampleServer(const char* name,
                                     UInt_t negotiationport,
                                     Bool_t blockingmode)
: TGo4ServerTask(name, negotiationport, blockingmode,kFALSE)
{
   TRACE((15,"TGo4ExampleServer::TGo4ExampleServer(Text_t*, Bool_t) constructor",__LINE__, __FILE__));

   TGo4Log::Debug(" ExampleServer ''%s'' started ",GetName());

   fcControlName= new Text_t[TGo4ThreadManager::fguTEXTLENGTH];
   fcLoggingName= new Text_t[TGo4ThreadManager::fguTEXTLENGTH];
   Text_t namebuffer[TGo4ThreadManager::fguTEXTLENGTH];

   fxController= new TGo4ExampleController();


   snprintf(namebuffer,TGo4ThreadManager::fguTEXTLENGTH-1,"ControllerRunnable of %s",GetName());
   TGo4ControllerRunnable* controlrun= new TGo4ControllerRunnable(namebuffer, this);
   snprintf(namebuffer,TGo4ThreadManager::fguTEXTLENGTH-1,"LoggerRunnable of %s",GetName());
   TGo4LoggingRunnable* logrun= new TGo4LoggingRunnable(namebuffer, this);

      // adding runnables to thread handler who takes over the responsibility...:

   snprintf(fcControlName,TGo4ThreadManager::fguTEXTLENGTH-1,"%s%s",fgcCONTROLTHREAD,GetName());
   fxWorkHandler->NewThread(fcControlName,controlrun);

   snprintf(fcLoggingName,TGo4ThreadManager::fguTEXTLENGTH-1,"%s%s",fgcLOGGINGTHREAD,GetName());
   fxWorkHandler->NewThread(fcLoggingName,logrun);

   Launch();
}

TGo4TaskHandlerCommandList* TGo4ExampleServer::CreateCommandList()
{
   return (new TGo4ExampleCommandList);
}

Int_t TGo4ExampleServer::StopWorkThreads()
{

   TGo4Log::Debug(" Example Server is stopping work threads ");
   Int_t rev=0;
   TGo4ThreadHandler* threadhandler= GetWorkHandler();
   TGo4BufferQueue* dataq=GetDataQueue();
   TGo4BufferQueue* statusq=GetStatusQueue();
  // stop my own threads, put dummies into queues to release semaphores
   threadhandler->Stop(fcControlName);
   if(dataq)
      {
         dataq->AddBufferFromObject(new TNamed("StopObject","dummy"));
         //cout << "added dummy data"<<endl;
      }
   else
      {
         //cout << "NO data queue"<<endl;
      }
   threadhandler->Stop(fcLoggingName);
   if(statusq)
      {
         statusq->AddBufferFromObject(new TGo4Status("StopStatus"));
         //cout << "added dummy status"<<endl;
      }
   else
      {
      //       cout << "NO status queue"<<endl;
      }
   rev=TGo4ServerTask::StopWorkThreads(); // this will set server task internal flag
   return rev;
}


Int_t TGo4ExampleServer::StartWorkThreads()
{

   TGo4Log::Debug(" Example Server is starting work threads ");
   Int_t rev=0;
    // start again the  working threads on next current task
   TGo4ThreadHandler* threadhandler= GetWorkHandler();
   threadhandler->Start(fcControlName);
   threadhandler->Start(fcLoggingName);
   rev=TGo4ServerTask::StartWorkThreads(); // this will set server task internal flag
   return rev;

}
