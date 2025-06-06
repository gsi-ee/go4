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

#include "TGo4StartException.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ThreadHandler.h"
#include "TGo4Runnable.h"

TGo4StartException::TGo4StartException(const TGo4StartException &right)
  :TGo4ThreadException(right)
{
   GO4TRACE((14,"TGo4StartException::TGo4StartException(const TGo4StartException &)",__LINE__, __FILE__));
}

TGo4StartException::TGo4StartException(TGo4Runnable *runnable, const char *threadname)
: TGo4ThreadException(runnable,threadname)
{
   GO4TRACE((14,"TGo4StartException::TGo4StartException(TGo4Runnable *, const char *)",__LINE__, __FILE__));
   fxDescription = "!!!-- Go4 Start Exception --!!!";
}


TGo4StartException::~TGo4StartException()
{
  GO4TRACE((14,"TGo4StartException::~TGo4StartException()",__LINE__, __FILE__));
}


TGo4StartException & TGo4StartException::operator=(const TGo4StartException &right)
{
   GO4TRACE((14, "TGo4StartException::operator=", __LINE__, __FILE__));
   if (&right != this)
      TGo4ThreadException::operator=(right); // copy base class members
   return *this;
}

Int_t TGo4StartException::Handle ()
{
   GO4TRACE((14, "TGo4StartException::Handle()", __LINE__, __FILE__));
   Int_t rev = -1;
   if (!GetThreadName()) {
      // no threadname specified, operate on thread of runnable
      GO4TRACE((13, "TGo4StartException::Handle() -- starting thread of runnable", __LINE__, __FILE__));
      rev = fxRunnable->GetThread()->Start();
   } else {
      // threadname given, use thread handler to cancel
      GO4TRACE((13, "TGo4StartException::Handle() -- starting thread by name", __LINE__, __FILE__));
      rev = fxThreadHandler->Start(GetThreadName());
   }
   // note: return values of Start() are boolean, are casted implicitly
   return rev;
}
