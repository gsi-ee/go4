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

#include "TGo4LocalCommandRunnable.h"

#include "TGo4Log.h"
#include "TGo4ServerTask.h"
#include "TGo4Command.h"
#include "TGo4CommandInvoker.h"
#include "TGo4Thread.h"
#include "TGo4ObjectQueue.h"

TGo4LocalCommandRunnable::TGo4LocalCommandRunnable() :
   TGo4Runnable(),
   fxLocalTask(0)
{
}

TGo4LocalCommandRunnable::TGo4LocalCommandRunnable(const char* name, TGo4Task* task) :
   TGo4Runnable(name,task),
   fxLocalTask(task)
{
   TGo4ObjectQueue* fxQueue = 0;
   if (fxLocalTask!=0)
     fxQueue = fxLocalTask->GetLocalCommandQueue();
}

TGo4LocalCommandRunnable::~TGo4LocalCommandRunnable()
{
}

Int_t TGo4LocalCommandRunnable::Run(void* ptr)
{
   TGo4ObjectQueue* fxQueue = 0;
   if (fxLocalTask!=0)
     fxQueue = fxLocalTask->GetLocalCommandQueue();
   if(fxQueue!=0) {
      TGo4Command* com= dynamic_cast<TGo4Command*> (fxQueue->WaitObject());
      if(com!=0) {
         if( com->GetCommandID() != TGo4Task::Get_fgiTERMID() ) {
            // normal operation if we have not a terminate dummy command
            //TGo4LockGuard mainlock; // protect command invocation!
            TGo4CommandInvoker::Instance()->Invoke(com); // execute command by invoker
          } else {
            // terminate dummy command: do not execute, but stop this thread
            GetThread()->Stop();
          }
          delete com;
      }
   }
   return 0;
}
