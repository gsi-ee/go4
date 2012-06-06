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

#include "TGo4TaskHandlerAbortException.h"

#include "go4iostream.h"

#include "TApplication.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4Task.h"
#include "TGo4TaskHandler.h"
#include "TGo4TaskHandlerRunnable.h"
#include "TGo4Slave.h"
#include "TGo4Master.h"
#include "TGo4ComRemoveClient.h"
#include "TGo4ComDisconnectSlave.h"

TGo4TaskHandlerAbortException::TGo4TaskHandlerAbortException (TGo4TaskHandler* taskhandler)
: TGo4TaskHandlerException(taskhandler)
{
   fxDescription= "!!!-- Go4 TaskHandler Abort Exception --!!!";
}

TGo4TaskHandlerAbortException::TGo4TaskHandlerAbortException(TGo4TaskHandlerRunnable * run)
: TGo4TaskHandlerException(run)
{
   fxDescription= "!!!-- Go4 TaskHandler Abort Exception --!!!";
}

TGo4TaskHandlerAbortException::~TGo4TaskHandlerAbortException()
{
}

Int_t TGo4TaskHandlerAbortException::Handle()
{
   if(fxCaller)
      {
         // we know the calling thread, stop it!
         fxCaller->GetThread()->Stop();
      }
   else
      {
         // no caller specified, continue
      }

   if (fxTaskHandler->IsAborting())
      {
         // a concurrent exception is already performing, we do nothing
         TGo4Log::Debug(" TaskHandlerAbortException: taskhandler is already aborting ");
         return 0;
      }
   else
      {
         // just go on with the show
         fxTaskHandler->SetAborting(kTRUE);
      } // if (fxTaskHandler->IsAborting())

   if (fxThreadManager==0)
         {
            // error
            fxTaskHandler->SetAborting(kFALSE);
            return -1;
         }
      else
         {
            // continue
         }  //   if (fxThreadManager==0)

   const char* taskname = fxTaskHandler->GetName();
   TGo4Master* master=0;
   TGo4Slave* slave=0;
   TGo4Task* task = dynamic_cast<TGo4Task*> (fxThreadManager);
   if(task)
      {
         master=task->GetMaster();
         slave=task->GetSlave();
      }
   if(slave)
      {
         TGo4Log::Debug(" TaskHandlerAbortException for slave %s ... ",taskname);
         if(slave->IsServer())
            {
               // note: we need a local command  here (executed in local thread),
               // since this is running inside thread of taskhandler to be deleted!
               TGo4ComRemoveClient* removecommand = new TGo4ComRemoveClient;
               //removecommand->SetClient("currentclient");
               removecommand->SetClient(taskname);
               removecommand->SetWaitForClient(kFALSE);
               TGo4Log::Debug(" TaskHandlerAbortException on slave server: revoming client %s",taskname);
               task->SubmitCommand(removecommand);
            }
         else
            {
               TGo4Log::Debug(" TaskHandlerAbortException: Terminating slave...");
               fxThreadManager->SetBeingQuit(kTRUE); // flag for the application that we expect to be quit soon
               cout <<"ttttttttttt TaskHandlerAbortException set the being quit" << endl;
               slave->TerminateFast();
            }
      }
   else if(master)
      {
         TGo4Log::Debug(" TaskHandlerAbortException: Removing current slave... ");
         // note: we need a local command here (executed in local thread), since
         // this is running inside thread of taskhandler to be deleted!
         TGo4ComDisconnectSlave* discommand = new TGo4ComDisconnectSlave;
         discommand->SetSlave("currentclient");
         discommand->SetWaitForSlave(kFALSE);
         TGo4Log::Debug(" TaskHandlerAbortException: Disconnecting current slave");
         task->SubmitCommand(discommand);
      }
   else if(task)
      {
         // no master, no slave: old style connection, test client and server role
         if(fxTaskHandler->IsClientMode())
            {
               // exception was raised by client task handler
               TGo4Log::Debug(" TaskHandlerAbortException: Quit client %s ... ",taskname);
               fxThreadManager->SetBeingQuit(kTRUE); // flag for the application that we expect to be quit soon
               task->TerminateFast();
            }
         else
            {
               // exception was raised by server task handler
               TGo4ComRemoveClient* removecommand = new TGo4ComRemoveClient;
               removecommand->SetClient(taskname);
               removecommand->SetWaitForClient(kFALSE);
               TGo4Log::Debug(" TaskHandlerAbortException: Disconnecting client %s ... ",taskname);
               task->SubmitCommand(removecommand);
               // we cannot remove our own thread, so use local command thread
            } // if (fxTaskHandler->IsClientMode())
      }
   else
      {
         // no task: something is very wrong!
         gApplication->Terminate();
      }
    return 0;
}

