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

#include "TGo4RemoveException.h"

#include "TThread.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ThreadHandler.h"
#include "TGo4Runnable.h"

TGo4RemoveException::TGo4RemoveException(const TGo4RemoveException &right) : TGo4ThreadException(right)
{
   GO4TRACE((14, "TGo4RemoveException::TGo4RemoveException(const TGo4RemoveException &)", __LINE__, __FILE__));
}

TGo4RemoveException::TGo4RemoveException(TGo4Runnable *runnable, const char *threadname)
   : TGo4ThreadException(runnable, threadname)
{
   GO4TRACE((14, "TGo4RemoveException::TGo4RemoveException(TGo4Runnable *, const char *)", __LINE__, __FILE__));
   fxDescription = "!!!-- Go4 Remove Exception --!!!";
}

TGo4RemoveException::~TGo4RemoveException()
{
   GO4TRACE((14, "TGo4RemoveException::~TGo4RemoveException()", __LINE__, __FILE__));
}

TGo4RemoveException &TGo4RemoveException::operator=(const TGo4RemoveException &right)
{
   GO4TRACE((14, "TGo4RemoveException::operator=", __LINE__, __FILE__));
   if (&right != this)
      TGo4ThreadException::operator=(right); // copy base class members
   return *this;
}

Int_t TGo4RemoveException::Handle()
{
   GO4TRACE((14, "TGo4RemoveException::Handle()", __LINE__, __FILE__));
   TString name;
   if (!GetThreadName()) {
      // no threadname specified, use name of thread associated with runnable
      GO4TRACE((13, "TGo4RemoveException::Handle() -- removing thread associated with runnable", __LINE__, __FILE__));
      name = fxRunnable->GetThread()->GetName();
   } else {
      // threadname given, use thread handler to cancel
      GO4TRACE((13, "TGo4RemoveException::Handle() -- removing thread by specified name", __LINE__, __FILE__));
      name = GetThreadName();
   }
   fxThreadHandler->RemoveThread(name.Data());
   TThread::CancelPoint();
   return 0;
}
