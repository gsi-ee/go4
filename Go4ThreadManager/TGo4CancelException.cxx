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

#include "TGo4CancelException.h"

#include "TThread.h"
#include "TGo4Thread.h"

#include "TGo4Log.h"
#include "TGo4ThreadHandler.h"
#include "TGo4Runnable.h"

TGo4CancelException::TGo4CancelException(const TGo4CancelException &right)
  :TGo4ThreadException(right)
{
   GO4TRACE((14,"TGo4CancelException::TGo4CancelException() copy constructor",__LINE__, __FILE__));
}

TGo4CancelException::TGo4CancelException (TGo4Runnable* runnable, const char* threadname)
: TGo4ThreadException(runnable, threadname)
{
   GO4TRACE((14,"TGo4CancelException::TGo4CancelException(TGo4Runnable*, const char*) constructor",__LINE__, __FILE__));
   fxDescription= "!!!-- Go4 Cancel Exception --!!!";
}


TGo4CancelException::~TGo4CancelException()
{
  GO4TRACE((14,"TGo4CancelException::~TGo4CancelException() destructor",__LINE__, __FILE__));
}


TGo4CancelException & TGo4CancelException::operator=(const TGo4CancelException &right)
{
   GO4TRACE((14,"TGo4CancelException::operator=",__LINE__, __FILE__));
   if (&right!=this)
      {
         GO4TRACE((13,"TGo4CancelException::operator= processing copy",__LINE__, __FILE__));
         TGo4ThreadException::operator=(right); // copy base class members
         // put additional member copies here...
         return *this;
      }
   else
      {
         // copy is already source object
         GO4TRACE((13,"TGo4CancelException::operator= source and destination objects are identical",__LINE__, __FILE__));
         return *this;
      }
}

Int_t TGo4CancelException::Handle()
{
   GO4TRACE((14,"TGo4CancelException::Handle()",__LINE__, __FILE__));
   if(GetThreadName()==0)
      // no threadname specified, operate on thread of runnable
      {
         GO4TRACE((13,"TGo4CancelException::Handle() -- canceling thread of runnable",__LINE__, __FILE__));
         fxRunnable->GetThread()->Cancel();
      }
   else
      // threadname given, use thread handler to cancel
      {
         GO4TRACE((13,"TGo4CancelException::Handle() -- canceling thread by name",__LINE__, __FILE__));
         fxThreadHandler->Cancel(GetThreadName());
      }
   TThread::CancelPoint(); // if thread cancels itself, this is necessary for proper cancellation:
   return 0;
}
