#include "TGo4RestartException.h"

#include "TThread.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ThreadHandler.h"
#include "TGo4Runnable.h"

TGo4RestartException::TGo4RestartException(const TGo4RestartException &right)
  :TGo4ThreadException(right)
{
   TRACE((14,"TGo4RestartException::TGo4RestartException() copy constructor",__LINE__, __FILE__));
}

TGo4RestartException::TGo4RestartException (TGo4Runnable* runnable, const Text_t* threadname)
: TGo4ThreadException(runnable,threadname)
{
   TRACE((14,"TGo4RestartException::TGo4RestartException(TGo4Runnable*, const Text_t*) constructor",__LINE__, __FILE__));
   fxDescription= "!!!-- Go4 Restart Exception --!!!";
}


TGo4RestartException::~TGo4RestartException()
{
  TRACE((14,"TGo4RestartException::~TGo4RestartException() destructor",__LINE__, __FILE__));
}


TGo4RestartException & TGo4RestartException::operator=(const TGo4RestartException &right)
{
   TRACE((14,"TGo4RestartException::operator=",__LINE__, __FILE__));
   if (&right!=this)
      {
         TRACE((13,"TGo4RestartException::operator= processing copy",__LINE__, __FILE__));
         TGo4ThreadException::operator=(right); // copy base class members
         // put additional member copies here...
         return *this;
      }
   else
      {
         // copy is already source object
         TRACE((13,"TGo4RestartException::operator= source and destination objects are identical",__LINE__, __FILE__));
         return *this;
      }
}

Int_t TGo4RestartException::Handle ()
{
   TRACE((14,"TGo4RestartException::Handle()",__LINE__, __FILE__));
   if(GetThreadName()==0)
      // no threadname specified, operate on thread of runnable
      {
         TRACE((13,"TGo4RestartException::Handle() -- recreating thread of runnable",__LINE__, __FILE__));
         fxRunnable->GetThread()->ReCreate();
      }
   else
      // threadname given, use thread handler to cancel
      {
         TRACE((13,"TGo4RestartException::Handle() -- recreating thread by name",__LINE__, __FILE__));
         fxThreadHandler->ReCreate(GetThreadName());
      }
   TThread::CancelPoint(); // if thread restarts itself, this is necessary for proper cancellation:
   return 0;
}
