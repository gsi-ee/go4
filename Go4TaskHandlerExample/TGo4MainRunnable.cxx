// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4MainRunnable.h"

#include "TGo4Log.h"
#include "TGo4LockGuard.h"
#include "TGo4Thread.h"

#include "TGo4CommandInvoker.h"
#include "TGo4LogicException.h"

TGo4MainRunnable::TGo4MainRunnable(const char *name, TGo4ExampleClient* cli)
: TGo4Runnable(name,cli)
{
   GO4TRACE((15,"TGo4MainRunnable::TGo4MainRunnable(const char *,TGo4ExampleClient *)",__LINE__, __FILE__));
   fxApplication=cli->GetApplication();
}

TGo4MainRunnable::~TGo4MainRunnable()
{
   GO4TRACE((15,"TGo4MainRunnable::~TGo4MainRunnable()",__LINE__, __FILE__));
}

Int_t TGo4MainRunnable::Run(void *)
{
   GO4TRACE((12,"TGo4MainRunnable::Run()",__LINE__, __FILE__));
   // first test: simply wait for command, then execute it and send back an object
   TGo4ExampleClient* cli= dynamic_cast<TGo4ExampleClient*> (fxManager);
   if(!cli)
     throw TGo4LogicException();
   TGo4Command *com = cli->NextCommand();
   if (com) {
      TGo4LockGuard mainlock;                      // protect command invocation!
      TGo4CommandInvoker::Instance()->Invoke(com); // execute command by invoker
      delete com;
   }
   // do "main event loop"...
   fxApplication->FillHistograms();
   TGo4Thread::Sleep(20);
   return 0;
}
