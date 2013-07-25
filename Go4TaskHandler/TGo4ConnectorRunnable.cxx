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

#include "TGo4ConnectorRunnable.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ServerTask.h"
#include "TGo4TaskManager.h"

TGo4ConnectorRunnable::TGo4ConnectorRunnable(const char* name, TGo4ServerTask* man)
   :TGo4Runnable(name,man)
{
   fxTaskManager=man->GetTaskManager();
}

TGo4ConnectorRunnable::~TGo4ConnectorRunnable()
{
}

Int_t TGo4ConnectorRunnable::Run(void* ptr)
{
   // create new taskhandler and connect server to  client:
   Int_t rev= fxTaskManager->ServeClient();
   if(rev< -1)
      {
         //std::cout <<"TGo4ConnectorRunnable::Run sees rev="<<rev << std::endl;
         fxGo4Thread->Stop(); // stop runnable for termination
      }
   else
      {
         // go on
      }
   return rev;
}
