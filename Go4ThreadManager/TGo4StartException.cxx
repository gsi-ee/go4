#include "TGo4StartException.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ThreadHandler.h"
#include "TGo4Runnable.h"

TGo4StartException::TGo4StartException(const TGo4StartException &right)
  :TGo4ThreadException(right)
{
   TRACE((14,"TGo4StartException::TGo4StartException() copy constructor",__LINE__, __FILE__));
}

TGo4StartException::TGo4StartException (TGo4Runnable* runnable, const Text_t* threadname)
: TGo4ThreadException(runnable,threadname)
{
   TRACE((14,"TGo4StartException::TGo4StartException(TGo4Runnable*, const Text_t*) constructor",__LINE__, __FILE__));
   fxDescription= "!!!-- Go4 Start Exception --!!!";
}


TGo4StartException::~TGo4StartException()
{
  TRACE((14,"TGo4StartException::~TGo4StartException() destructor",__LINE__, __FILE__));
}


TGo4StartException & TGo4StartException::operator=(const TGo4StartException &right)
{
   TRACE((14,"TGo4StartException::operator=",__LINE__, __FILE__));
   if (&right!=this)
      {
         TRACE((13,"TGo4StartException::operator= processing copy",__LINE__, __FILE__));
         TGo4ThreadException::operator=(right); // copy base class members
         // put additional member copies here...
         return *this;
      }
   else
      {
         // copy is already source object
         TRACE((13,"TGo4StartException::operator= source and destination objects are identical",__LINE__, __FILE__));
         return *this;
      }
}

Int_t TGo4StartException::Handle ()
{
   TRACE((14,"TGo4StartException::Handle()",__LINE__, __FILE__));
   Int_t rev=-1;
   if(GetThreadName()==0)
      // no threadname specified, operate on thread of runnable
      {
         TRACE((13,"TGo4StartException::Handle() -- starting thread of runnable",__LINE__, __FILE__));
         rev=fxRunnable->GetThread()->Start();
      }
   else
      // threadname given, use thread handler to cancel
      {
         TRACE((13,"TGo4StartException::Handle() -- starting thread by name",__LINE__, __FILE__));
         rev=fxThreadHandler->Start(GetThreadName());
      }
   // note: return values of Start() are boolean, are casted implicitly
   return rev;
}




