#include "TGo4ThreadException.h"

#include "TGo4Log.h"
#include "TGo4ThreadManager.h"
#include "TGo4ThreadHandler.h"
#include "TGo4Runnable.h"

TGo4ThreadException::TGo4ThreadException(const TGo4ThreadException &right)
:TGo4ControlException(right)
{
   TRACE((14,"TGo4ThreadException::TGo4ThreadException() copy constructor",__LINE__, __FILE__));
      fxThreadName=right.fxThreadName;
      fxThreadHandler=right.fxThreadHandler;
      fxRunnable=right.fxRunnable;
}

TGo4ThreadException::TGo4ThreadException (TGo4Runnable* runnable, const Text_t* threadname)
{
   TRACE((14,"TGo4ThreadException::TGo4ThreadException(TGo4Runnable*, const Text_t*) constructor",__LINE__, __FILE__));
   fxRunnable=runnable;
   TGo4ThreadManager* man =fxRunnable->GetThreadManager();
   fxThreadHandler=man->GetWorkHandler();
   if(threadname==0)
      // default: use thread belonging to runnable
      {
      TRACE((13,"TGo4ThreadException constructor -- no threadname specified",__LINE__, __FILE__));

         fxThreadName="0"; // class member is TString...
      }
   else
      {
      // access to thread by name
      TRACE((13,"TGo4ThreadException constructor -- using given threadname",__LINE__, __FILE__));
          fxThreadName=threadname;
      }
}


TGo4ThreadException::~TGo4ThreadException()
{
  TRACE((14,"TGo4ThreadException::~TGo4ThreadException() destructor",__LINE__, __FILE__));
}


TGo4ThreadException & TGo4ThreadException::operator=(const TGo4ThreadException &right)
{
     TRACE((14,"TGo4ThreadException::operator=",__LINE__, __FILE__));
 if (&right!=this)
    {
      TRACE((13,"TGo4ThreadException::operator= processing copy",__LINE__, __FILE__));
      TGo4ControlException::operator=(right); // copy base class members
      // put additional member copies here...
      fxThreadName=right.fxThreadName;
      fxThreadHandler=right.fxThreadHandler;
      fxRunnable=right.fxRunnable;
      return *this;
    }
  else
    {
      // copy is already source object
      TRACE((13,"TGo4ThreadException::operator= source and destination objects are identical",__LINE__, __FILE__));
      return *this;
    }
}

const Text_t* TGo4ThreadException::GetThreadName ()
{
   TRACE((14,"TGo4ThreadException::GetThreadName ()",__LINE__, __FILE__));
   if(fxThreadName=="0")
      // no, TString was set to zero character:
      {
         TRACE((14,"TGo4ThreadException::GetThreadName () -- name not specified",__LINE__, __FILE__));
         return 0;
      }
   else
      // yes, TString contains given thread name
      {
         TRACE((14,"TGo4ThreadException::GetThreadName () -- found thread name",__LINE__, __FILE__));
         return fxThreadName.Data();
      }
}





