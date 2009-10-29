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

#include "TGo4DisplayLoggerTimer.h"

#include "TGo4Log.h"
#include "TGo4Task.h"
#include "TGo4Display.h"

TGo4DisplayLoggerTimer::TGo4DisplayLoggerTimer (TGo4Display* display, Long_t msperiod) :
   TTimer(msperiod,kTRUE),fxDisplay(display)
{
   TGo4Log::Debug(" Display logger Timer is started with %d ms period.",msperiod);
}

TGo4DisplayLoggerTimer::~TGo4DisplayLoggerTimer()
{
}

Bool_t TGo4DisplayLoggerTimer::Notify ()
{
   TGo4Status* stat=0;
   TGo4Task* task=fxDisplay->GetTask();
   while((stat=task->NextStatus(kFALSE))!=0) {
      //TGo4Log::Debug(" Display logger Timer displaying log...");
      fxDisplay->DisplayLog(stat);
   }
   Reset();
   return kFALSE;
}
