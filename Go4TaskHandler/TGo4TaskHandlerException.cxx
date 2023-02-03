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

#include "TGo4TaskHandlerException.h"

#include "TGo4TaskHandler.h"
#include "TGo4TaskHandlerRunnable.h"

TGo4TaskHandlerException::TGo4TaskHandlerException (TGo4TaskHandler *taskhandler)
{
   fxDescription= "!!!-- Go4 TaskHandler Exception --!!!";
   fxTaskHandler = taskhandler;
   if(fxTaskHandler)
      {
         // o.k. we have a given taskhandler
         fxThreadManager = fxTaskHandler->GetThreadManager();
      }
   else
      {
         // wrong call with zero pointer: dummy init of manager, will be checked in Handle()
         fxThreadManager = nullptr;
      }
}

TGo4TaskHandlerException::TGo4TaskHandlerException(TGo4TaskHandlerRunnable* run)
{
   fxDescription= "!!!-- Go4 TaskHandler Exception --!!!";
   fxCaller = run;
   if(fxCaller)
      {
         fxTaskHandler = fxCaller->GetTaskHandler();
      }
   else
      {
         fxTaskHandler = nullptr;
      }

   if(fxTaskHandler)
      {
         // o.k. we have a given taskhandler
         fxThreadManager = fxTaskHandler->GetThreadManager();
      }
   else
      {
         // wrong call with zero pointer: dummy init of manager, will be checked in Handle()
         fxThreadManager = nullptr;
      }
}

TGo4TaskHandlerException::~TGo4TaskHandlerException()
{
}
