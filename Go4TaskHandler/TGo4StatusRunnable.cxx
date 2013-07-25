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

#include "TGo4StatusRunnable.h"

#include <signal.h>

#include "TGo4Log.h"
#include "TGo4ThreadManager.h"
#include "TGo4Thread.h"
#include "TGo4BufferQueue.h"
#include "TGo4Socket.h"
#include "TGo4SocketSignalHandler.h"
#include "TGo4TaskHandler.h"
#include "TGo4TaskHandlerAbortException.h"

TGo4StatusRunnable::TGo4StatusRunnable(const char* name, TGo4ThreadManager* man, TGo4TaskHandler* hand, Bool_t receivermode) :
   TGo4TaskHandlerRunnable(name,man,hand,receivermode)
{
   fxBufferQueue = dynamic_cast<TGo4BufferQueue*> (fxTaskHandler->GetStatusQueue() );
   fxTransport = fxTaskHandler->GetStatusTransport();
}


TGo4StatusRunnable::~TGo4StatusRunnable()
{
}

Int_t TGo4StatusRunnable::Run(void* ptr)
{
   if(!CheckTransportOpen()) return 0;
   if(fbReceiverMode)
      {
      // server side: receive status objects from client
      Int_t  rev = fxTransport->ReceiveBuffer();
      if(rev>=0)
         {
            TBuffer* buf=const_cast<TBuffer*> (fxTransport->GetBuffer());
            if(CheckStopBuffer(buf)) return 0; // stop for disconnect mode
            fxBufferQueue->AddBuffer(buf, kTRUE);
         } ////if(rev>=0)
      else
         {
            if (TGo4SocketSignalHandler::IsLastSignalWINCH())
               {
                  // TSocket error because of window resize, do not abort!
                  TGo4Log::Debug(" %s: caught SIGWINCH ",GetName());
                  TGo4SocketSignalHandler::SetLastSignal(0); // reset
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
                   //GetThread()->Stop(); std::cout <<"Stopped status runnable. no termination" << std::endl;
                  throw TGo4TaskHandlerAbortException(this);
               }
         }

   } ////if(fbReceiverMode)
   else
      {
      // client side: send status buffer to server
         // get next status buffer from queue or wait for it
         TBuffer* buf= fxBufferQueue->WaitBuffer();
         if (buf)
            // check if there is really an object from queue
            {
               CheckStopBuffer(buf);
               fxTransport->SendBuffer(buf);
               fxBufferQueue->FreeBuffer(buf);
            }
      } // else if(fbReceiverMode)


return 0;
}

