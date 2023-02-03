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

#include "TGo4ReplaceException.h"

#include "TThread.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ThreadHandler.h"
#include "TGo4Runnable.h"

TGo4ReplaceException::TGo4ReplaceException(const TGo4ReplaceException &right)
  :TGo4ThreadException(right)
{
   GO4TRACE((14,"TGo4ReplaceException::TGo4ReplaceException() copy constructor",__LINE__, __FILE__));
   fxNewRunnable=right.fxNewRunnable;
}

TGo4ReplaceException::TGo4ReplaceException (TGo4Runnable *runnable, TGo4Runnable *newrunnable, const char *oldthreadname)
 :TGo4ThreadException(runnable, oldthreadname)
{
   GO4TRACE((14,"TGo4ReplaceException::TGo4ReplaceException(TGo4Runnable *,TGo4Runnable *,const char*) constructor",__LINE__, __FILE__));
   fxDescription = "!!!-- Go4 Replace Exception --!!!";
   if(!newrunnable) {
      GO4TRACE((13,"TGo4ReplaceException constructor -- new runnable same as old runnable",__LINE__, __FILE__));
      fxNewRunnable = (TGo4Runnable *) runnable->Clone(); // deep copy
      *fxNewRunnable=*runnable;   // pointers to manager and thread are not streamed,
   } else {
      GO4TRACE((13,"TGo4ReplaceException constructor -- new runnable specified",__LINE__, __FILE__));
      fxNewRunnable = newrunnable;
   }
}


TGo4ReplaceException::~TGo4ReplaceException()
{
  GO4TRACE((14,"TGo4ReplaceException::~TGo4ReplaceException() destructor",__LINE__, __FILE__));
}


TGo4ReplaceException & TGo4ReplaceException::operator=(const TGo4ReplaceException &right)
{
   GO4TRACE((14,"TGo4ReplaceException::operator=",__LINE__, __FILE__));
   if (&right != this) {
      TGo4ThreadException::operator=(right); // copy base class members
      fxNewRunnable = right.fxNewRunnable;
   }
   return *this;
}

Int_t TGo4ReplaceException::Handle ()
{
   GO4TRACE((14,"TGo4ReplaceException::Handle()",__LINE__, __FILE__));
    // first step: find out name of thread to be replaced
   TString name;
   if(!GetThreadName())
      // no threadname specified, use name of thread associated with runnable
      {
         GO4TRACE((13,"TGo4ReplaceException::Handle() -- replacing thread associated with runnable",__LINE__, __FILE__));
         name = fxRunnable->GetThread()->GetName();
      }
   else
      // threadname given, use thread handler to cancel
      {
         GO4TRACE((13,"TGo4ReplaceException::Handle() -- replacing thread by specified name",__LINE__, __FILE__));
         name = GetThreadName();
      }

      // second step: remove it
      fxThreadHandler->RemoveThread(name.Data());

    // third step: new thread
    fxThreadHandler->NewThread(name,fxNewRunnable);
    fxThreadHandler->Start(name);

    TThread::CancelPoint();
    return 0;

}

