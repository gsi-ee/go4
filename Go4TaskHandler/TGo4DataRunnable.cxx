// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4DataRunnable.h"

#include <signal.h>

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4BufferQueue.h"
#include "TGo4SocketSignalHandler.h"
#include "TGo4Socket.h"
#include "TGo4TaskHandler.h"
#include "TGo4TaskHandlerAbortException.h"
#include "TGo4Task.h"
#include "TGo4ComDisconnectSlave.h"

TGo4DataRunnable::TGo4DataRunnable(const char* name,
                                   TGo4ThreadManager* man,
                                   TGo4TaskHandler* hand,
                                   Bool_t receivermode)
   :TGo4TaskHandlerRunnable(name,man,hand,receivermode)
{
   fxBufferQueue=dynamic_cast<TGo4BufferQueue*> (fxTaskHandler->GetDataQueue() );
   fxTransport=fxTaskHandler->GetDataTransport();
}

TGo4DataRunnable::~TGo4DataRunnable()
{
}

Int_t TGo4DataRunnable::Run(void* ptr)
{
   if(!CheckTransportOpen()) return 0;
   if(fbReceiverMode)
      {
      Int_t rev=fxTransport->ReceiveBuffer();
      if(rev>=0)
         {
            TBuffer* buf=const_cast<TBuffer*> (fxTransport->GetBuffer());
            Int_t val=0;
            if(CheckStopBuffer(buf,&val)) return 0; // stop for disconnect mode
            Go4EmergencyCommand_t comvalue= (Go4EmergencyCommand_t) (val);
            if(val>=0 && comvalue==kComQuit)
               {
                  //std::cout <<"QQQQQQQQQ Data Runnable has QUIT "<< std::endl;
                  GetThread()->Stop();
                  TGo4Command* qcommand = new TGo4ComDisconnectSlave;
                  TGo4Task* cli = dynamic_cast<TGo4Task*>(fxManager);
                  if(cli) cli->SubmitLocalCommand(qcommand);
                  return 0;
                }
            else
               {
                  fxBufferQueue->AddBuffer(buf, kTRUE);
               }
          } //// if(rev>=0)
      else
         {
            // error
             if (TGo4SocketSignalHandler::IsLastSignalWINCH())
             //if (TGo4SocketSignalHandler::fgxLastSignal == kSigWindowChanged)
               {
                  // TSocket error because of window resize, do not abort!
                  TGo4Log::Debug(" %s: caught SIGWINCH ",GetName());
                  TGo4SocketSignalHandler::SetLastSignal(0); // reset
                  //TGo4SocketSignalHandler::fgxLastSignal = (ESignals) 0;
               }
            else if(fxManager->IsTerminating())
               {
                  //std::cout <<"Receive error in "<<GetName()<< " while threadmanager is terminating. Ignored!" << std::endl;
                  TGo4Log::Debug("Receive Error in %s during threadmanager termination. Ignored.",GetName());
                  GetThread()->Stop();

               }
            else
               {
                  TGo4Log::Debug(" !!!Receive Error in %s!!!",
                     GetName());
                  //GetThread()->Stop(); std::cout <<"Stopped data runnable. no termination" << std::endl;
                  throw TGo4TaskHandlerAbortException(this);
               }

         } // end if(rev>=0)

      } ////if(fbReceiverMode)
   else
      {
         // get next command from queue or wait for it
         TBuffer* buf= fxBufferQueue->WaitBuffer();
         if (buf)
            // check if there is really an object from queue
            {
               CheckStopBuffer(buf);
               fxTransport->SendBuffer(buf);
               fxBufferQueue->FreeBuffer(buf); // will delete it or put it back into own free buffer list
            }
      } //// else if(fbReceiverMode)
   return 0;
}
