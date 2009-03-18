#include "TGo4Runnable.h"

#include "TThread.h"

#include "TGo4Log.h"
#include "TGo4Thread.h"
#include "TGo4ThreadManager.h"

TGo4Runnable::TGo4Runnable() :
   TNamed(),
   fxManager(0),
   fxGo4Thread(0)
{
}

TGo4Runnable::TGo4Runnable(const TGo4Runnable &right) :
   TNamed(right),
   fxManager(right.fxManager),
   fxGo4Thread(right.fxGo4Thread)
{
   TRACE((14,"TGo4Runnable::TGo4Runnable() copy constructor",__LINE__, __FILE__));
}

TGo4Runnable::TGo4Runnable (const char* name, TGo4ThreadManager* man) :
   TNamed(name,"This is a TGo4Runnable"), fxGo4Thread(0)
{
   TRACE((14,"TGo4Runnable::TGo4Runnable(Text_t*,TGo4ThreadManager*) constructor",__LINE__, __FILE__));
   fxManager=man;
}

TGo4Runnable::~TGo4Runnable()
{
   TRACE((14,"TGo4Runnable::~TGo4Runnable() destructor",__LINE__, __FILE__));
}

void TGo4Runnable::ThreadCatch (TGo4Exception& ex)
{
   TRACE((12,"TGo4Runnable::ThreadCatch()",__LINE__, __FILE__));

   TGo4Log::Debug("\n %s occured in Runnable``%s''(Thread``%s''PID:%d) \n",
      ex.What(),GetName(),fxGo4Thread->GetName(),fxGo4Thread->GetPID());
   ex.Handle(); // execute Exception own handler method
}

void TGo4Runnable::UnexpectedCatch ()
{
   TRACE((12,"TGo4Runnable::UnexpectedCatch()",__LINE__, __FILE__));

   TGo4Log::Debug("!!!-- Unexpected Exception --!!! occured in Runnable``%s''(Thread``%s''PID:%d) ",
      GetName(),fxGo4Thread->GetName(),fxGo4Thread->GetPID());
   fxManager->Terminate();
   TThread::CancelPoint();
   //gApplication->Terminate(0);
}

Int_t TGo4Runnable::PreRun (void* arg)
{
   TRACE((12,"TGo4Runnable::PreRun()",__LINE__, __FILE__));

   TGo4Log::Debug("Executing Runnable default PreRun Method \n");
   return 0;
}

Int_t TGo4Runnable::PostRun (void* arg)
{
   TRACE((12,"TGo4Runnable::PostRun()",__LINE__, __FILE__));

   TGo4Log::Debug("Executing Runnable default PostRun Method \n");
   return 0;
}
