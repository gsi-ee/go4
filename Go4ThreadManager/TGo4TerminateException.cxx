#include "TGo4TerminateException.h"

#include "TThread.h"

#include "TGo4Log.h"
#include "TGo4ThreadManager.h"
#include "TGo4Runnable.h"

TGo4TerminateException::TGo4TerminateException (TGo4Runnable* runnable)
{
 TRACE((14,"TGo4TerminateException::TGo4TerminateException (TGo4Runnable*)", __LINE__, __FILE__));
   fxDescription= "!!!-- Go4 Terminate Exception --!!!";
   fxThreadManager=runnable->GetThreadManager();
}

TGo4TerminateException::TGo4TerminateException (TGo4ThreadManager* man)
{
TRACE((14,"TGo4TerminateException::TGo4TerminateException (TGo4ThreadManager*)", __LINE__, __FILE__));
   fxDescription= "!!!-- Go4 Terminate Exception --!!!";
   fxThreadManager=man;
}

TGo4TerminateException::TGo4TerminateException(const TGo4TerminateException &right)
  :TGo4ControlException(right)
{
TRACE((14,"TGo4TerminateException::TGo4TerminateException (const TGo4TerminateException*)", __LINE__, __FILE__));
   fxThreadManager=right.fxThreadManager;
}

TGo4TerminateException & TGo4TerminateException::operator=(const TGo4TerminateException &right)
{
TRACE((14,"TGo4TerminateException::operator=(const TGo4TerminateException*)", __LINE__, __FILE__));
   if (&right!=this)
      {
         ;
         TGo4ControlException::operator=(right); // copy base class members
         // put additional member copies here...
         fxThreadManager=right.fxThreadManager;
         return *this;
      }
   else
      {
         // copy is already source object
         ;
         return *this;
      }
}

TGo4TerminateException::~TGo4TerminateException()
{
   TRACE((14,"TGo4TerminateException::~TGo4TerminateException()", __LINE__, __FILE__));

}

Int_t TGo4TerminateException::Handle ()
{
TRACE((12,"TGo4TaskHandlerAbortException::Handle()", __LINE__, __FILE__));

   TGo4Log::Debug("\n Terminate Exception default Handle: Terminating Thread Manager %s ... \n",
            fxThreadManager->GetName());
   fxThreadManager->Terminate();
   TThread::CancelPoint();
   return 0;

}
