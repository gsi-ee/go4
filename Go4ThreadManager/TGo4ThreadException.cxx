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

#include "TGo4ThreadException.h"

#include "TGo4Log.h"
#include "TGo4ThreadManager.h"
#include "TGo4Runnable.h"

TGo4ThreadException::TGo4ThreadException(const TGo4ThreadException &right)
:TGo4ControlException(right)
{
   GO4TRACE((14,"TGo4ThreadException::TGo4ThreadException(const TGo4ThreadException &)",__LINE__, __FILE__));
      fxThreadName=right.fxThreadName;
      fxThreadHandler=right.fxThreadHandler;
      fxRunnable=right.fxRunnable;
}

TGo4ThreadException::TGo4ThreadException(TGo4Runnable *runnable, const char *threadname)
{
   GO4TRACE((14,"TGo4ThreadException::TGo4ThreadException(TGo4Runnable *, const char *)",__LINE__, __FILE__));
   fxRunnable = runnable;
   TGo4ThreadManager *man = fxRunnable->GetThreadManager();
   fxThreadHandler = man->GetWorkHandler();
   if (!threadname) {
      // default: use thread belonging to runnable
      GO4TRACE((13, "TGo4ThreadException constructor -- no threadname specified", __LINE__, __FILE__));
      fxThreadName = "0"; // class member is TString...
   } else {
      // access to thread by name
      GO4TRACE((13, "TGo4ThreadException constructor -- using given threadname", __LINE__, __FILE__));
      fxThreadName = threadname;
   }
}

TGo4ThreadException::~TGo4ThreadException()
{
  GO4TRACE((14,"TGo4ThreadException::~TGo4ThreadException()",__LINE__, __FILE__));
}


TGo4ThreadException & TGo4ThreadException::operator=(const TGo4ThreadException &right)
{
   GO4TRACE((14, "TGo4ThreadException::operator=", __LINE__, __FILE__));
   if (&right != this) {
      TGo4ControlException::operator=(right); // copy base class members
      fxThreadName = right.fxThreadName;
      fxThreadHandler = right.fxThreadHandler;
      fxRunnable = right.fxRunnable;
   }
   return *this;
}

const char *TGo4ThreadException::GetThreadName() const
{
   GO4TRACE((14,"TGo4ThreadException::GetThreadName()",__LINE__, __FILE__));
   if (fxThreadName == "0") {
      // no, TString was set to zero character:
      GO4TRACE((14, "TGo4ThreadException::GetThreadName() -- name not specified", __LINE__, __FILE__));
      return nullptr;
   } else {
      // yes, TString contains given thread name
      GO4TRACE((14, "TGo4ThreadException::GetThreadName() -- found thread name", __LINE__, __FILE__));
      return fxThreadName.Data();
   }
}
