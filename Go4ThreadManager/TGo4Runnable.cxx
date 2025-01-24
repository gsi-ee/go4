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

#include "TGo4Runnable.h"

#include "TThread.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ThreadManager.h"
#include "TGo4Exception.h"


TGo4Runnable::TGo4Runnable(const TGo4Runnable &right) :
   TNamed(right),
   fxManager(right.fxManager),
   fxGo4Thread(right.fxGo4Thread)
{
   GO4TRACE((14,"TGo4Runnable::TGo4Runnable(const TGo4Runnable &)",__LINE__, __FILE__));
}

TGo4Runnable::TGo4Runnable(const char *name, TGo4ThreadManager *man) :
   TNamed(name,"This is a TGo4Runnable")
{
   GO4TRACE((14,"TGo4Runnable::TGo4Runnable(const char *,TGo4ThreadManager *)",__LINE__, __FILE__));
   fxManager = man;
}

TGo4Runnable::~TGo4Runnable()
{
   GO4TRACE((14,"TGo4Runnable::~TGo4Runnable()",__LINE__, __FILE__));
}

void TGo4Runnable::ThreadCatch(TGo4Exception &ex)
{
   GO4TRACE((12, "TGo4Runnable::ThreadCatch()", __LINE__, __FILE__));

   TGo4Log::Debug("\n %s occured in Runnable``%s''(Thread``%s''PID:%d) \n", ex.What(), GetName(),
                  fxGo4Thread->GetName(), fxGo4Thread->GetPID());
   ex.Handle(); // execute Exception own handler method
}

void TGo4Runnable::UnexpectedCatch()
{
   GO4TRACE((12,"TGo4Runnable::UnexpectedCatch()",__LINE__, __FILE__));

   TGo4Log::Debug("!!!-- Unexpected Exception --!!! occured in Runnable``%s''(Thread``%s''PID:%d) ",
      GetName(),fxGo4Thread->GetName(),fxGo4Thread->GetPID());
   fxManager->Terminate();
   TThread::CancelPoint();
   //gApplication->Terminate(0);
}

Int_t TGo4Runnable::PreRun(void *)
{
   GO4TRACE((12,"TGo4Runnable::PreRun()",__LINE__, __FILE__));

   TGo4Log::Debug("Executing Runnable default PreRun Method \n");
   return 0;
}

Int_t TGo4Runnable::PostRun(void *)
{
   GO4TRACE((12,"TGo4Runnable::PostRun()",__LINE__, __FILE__));

   TGo4Log::Debug("Executing Runnable default PostRun Method \n");
   return 0;
}
