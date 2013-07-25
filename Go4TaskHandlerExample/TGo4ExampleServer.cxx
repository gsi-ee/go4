// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4ExampleServer.h"

#include "TGo4Log.h"
#include "TGo4Status.h"
#include "TGo4BufferQueue.h"
#include "TGo4ThreadHandler.h"

#include "TGo4LoggingRunnable.h"
#include "TGo4ControllerRunnable.h"
#include "TGo4ExampleCommandList.h"

const char* TGo4ExampleServer::fgcLOGGINGTHREAD="LOGGER-";
const char* TGo4ExampleServer::fgcCONTROLTHREAD="CONTROL-";

TGo4ExampleController* TGo4ExampleServer::GetController()
{
   GO4TRACE((12,"TGo4ExampleServer::GetController()",__LINE__, __FILE__));

   return fxController;
}
TGo4ExampleServer::~TGo4ExampleServer()
{
   GO4TRACE((15,"TGo4ExampleServer::~TGo4ExampleServer() destructor",__LINE__, __FILE__));

   fxWorkHandler->CancelAll(); // make sure threads wont work on controller instance when its deleted
   delete fxController;

}
TGo4ExampleServer::TGo4ExampleServer(const char* name,
                                     UInt_t negotiationport,
                                     Bool_t blockingmode)
: TGo4ServerTask(name, negotiationport, blockingmode,kFALSE)
{
   GO4TRACE((15,"TGo4ExampleServer::TGo4ExampleServer(const char*, Bool_t) constructor",__LINE__, __FILE__));

   TGo4Log::Debug(" ExampleServer ''%s'' started ",GetName());

   fxController = new TGo4ExampleController();

   TGo4ControllerRunnable* controlrun= new TGo4ControllerRunnable(Form("ControllerRunnable of %s",GetName()), this);

   TGo4LoggingRunnable* logrun = new TGo4LoggingRunnable(Form("LoggerRunnable of %s",GetName()), this);

      // adding runnables to thread handler who takes over the responsibility...:

   fcControlName.Form("%s%s",fgcCONTROLTHREAD,GetName());
   fxWorkHandler->NewThread(fcControlName.Data(), controlrun);

   fcLoggingName.Form("%s%s",fgcLOGGINGTHREAD,GetName());
   fxWorkHandler->NewThread(fcLoggingName.Data(),logrun);

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
   threadhandler->Stop(fcControlName.Data());
   if(dataq)
      {
         dataq->AddBufferFromObject(new TNamed("StopObject","dummy"));
         //std::cout << "added dummy data"<< std::endl;
      }
   else
      {
         //std::cout << "NO data queue"<< std::endl;
      }
   threadhandler->Stop(fcLoggingName.Data());
   if(statusq)
      {
         statusq->AddBufferFromObject(new TGo4Status("StopStatus"));
         //std::cout << "added dummy status"<< std::endl;
      }
   else
      {
      //       std::cout << "NO status queue"<< std::endl;
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
   threadhandler->Start(fcControlName.Data());
   threadhandler->Start(fcLoggingName.Data());
   rev=TGo4ServerTask::StartWorkThreads(); // this will set server task internal flag
   return rev;

}
