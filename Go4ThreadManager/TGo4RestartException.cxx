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

#include "TGo4RestartException.h"

#include "TThread.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ThreadHandler.h"
#include "TGo4Runnable.h"

TGo4RestartException::TGo4RestartException(const TGo4RestartException &right)
  :TGo4ThreadException(right)
{
   GO4TRACE((14,"TGo4RestartException::TGo4RestartException() copy constructor",__LINE__, __FILE__));
}

TGo4RestartException::TGo4RestartException (TGo4Runnable* runnable, const char* threadname)
: TGo4ThreadException(runnable,threadname)
{
   GO4TRACE((14,"TGo4RestartException::TGo4RestartException(TGo4Runnable*, const char*) constructor",__LINE__, __FILE__));
   fxDescription= "!!!-- Go4 Restart Exception --!!!";
}


TGo4RestartException::~TGo4RestartException()
{
  GO4TRACE((14,"TGo4RestartException::~TGo4RestartException() destructor",__LINE__, __FILE__));
}


TGo4RestartException & TGo4RestartException::operator=(const TGo4RestartException &right)
{
   GO4TRACE((14,"TGo4RestartException::operator=",__LINE__, __FILE__));
   if (&right!=this)
      {
         GO4TRACE((13,"TGo4RestartException::operator= processing copy",__LINE__, __FILE__));
         TGo4ThreadException::operator=(right); // copy base class members
         // put additional member copies here...
         return *this;
      }
   else
      {
         // copy is already source object
         GO4TRACE((13,"TGo4RestartException::operator= source and destination objects are identical",__LINE__, __FILE__));
         return *this;
      }
}

Int_t TGo4RestartException::Handle ()
{
   GO4TRACE((14,"TGo4RestartException::Handle()",__LINE__, __FILE__));
   if(GetThreadName()==0)
      // no threadname specified, operate on thread of runnable
      {
         GO4TRACE((13,"TGo4RestartException::Handle() -- recreating thread of runnable",__LINE__, __FILE__));
         fxRunnable->GetThread()->ReCreate();
      }
   else
      // threadname given, use thread handler to cancel
      {
         GO4TRACE((13,"TGo4RestartException::Handle() -- recreating thread by name",__LINE__, __FILE__));
         fxThreadHandler->ReCreate(GetThreadName());
      }
   TThread::CancelPoint(); // if thread restarts itself, this is necessary for proper cancellation:
   return 0;
}
