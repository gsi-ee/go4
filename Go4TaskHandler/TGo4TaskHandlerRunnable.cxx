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

#include "TGo4TaskHandlerRunnable.h"

#include "TGo4Log.h"
#include "TGo4Socket.h"
#include "TGo4BufferQueue.h"
#include "TGo4TaskHandler.h"
#include "TGo4TaskHandlerAbortException.h"

#include "TGo4Thread.h"
#include "Go4EmergencyCommands.h"

TGo4TaskHandlerRunnable::TGo4TaskHandlerRunnable(const char* name, TGo4ThreadManager* man, TGo4TaskHandler* hand, Bool_t receivermode)
   :TGo4Runnable(name,man), fxTransport(0), fbReceiverMode(0)
{
   fxTaskHandler=hand;
   fbReceiverMode=receivermode;
}

TGo4TaskHandlerRunnable::~TGo4TaskHandlerRunnable()
{
}

TGo4TaskHandler* TGo4TaskHandlerRunnable::GetTaskHandler()
{
   return fxTaskHandler;
}

Bool_t TGo4TaskHandlerRunnable::CheckStopBuffer(TBuffer* buf, Int_t* result)
{
if(buf==0) return kFALSE;
//std::cout <<"CCCCCCCCCC CheckStopBuffer in "<< GetName() << std::endl;
Int_t val=TGo4BufferQueue::DecodeValueBuffer(buf);
if(result) *result=val;
if(val<0) return kFALSE; // no valid message in buffer
Go4EmergencyCommand_t comvalue= (Go4EmergencyCommand_t) (val);
if(comvalue==kComCloseInput)
   {
      //std::cout <<"CCCCCCCCCC CheckStopBuffer has close input in "<< GetName() << std::endl;
      GetThread()->Stop();
      return kTRUE;
   }
else if (comvalue==kComAbortTask)
   {
      //std::cout <<"CCCCCCCCCC CheckStopBuffer has task abort command in "<< GetName() << std::endl;
      TGo4Log::Debug(" !!!Receiving taskhandler abort buffer in %s !!!", GetName());
      throw TGo4TaskHandlerAbortException(this);

   }
else
   {
      return kFALSE;
   }
}

Bool_t TGo4TaskHandlerRunnable::CheckTransportOpen()
{
Bool_t open=kTRUE;
if(fxTransport==0)
   open=kFALSE;
else if(!fxTransport->IsOpen())
   open=kFALSE;
else
   open=kTRUE;
if(!open) TGo4Thread::Sleep(TGo4TaskHandler::Get_fguPORTWAITTIME());
          // avoid wild looping in Run() when socket is not open
return open;
}
