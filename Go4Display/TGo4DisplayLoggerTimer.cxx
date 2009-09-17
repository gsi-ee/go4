#include "TGo4DisplayLoggerTimer.h"

#include "TGo4Log.h"
#include "TGo4Task.h"
#include "TGo4Display.h"

TGo4DisplayLoggerTimer::TGo4DisplayLoggerTimer (TGo4Display* display, Long_t msperiod) :
   TTimer(msperiod,kTRUE),fxDisplay(display)
{
   TGo4Log::Debug(" Display logger Timer is started with %d ms period.",msperiod);
}

TGo4DisplayLoggerTimer::~TGo4DisplayLoggerTimer()
{
}

Bool_t TGo4DisplayLoggerTimer::Notify ()
{
   TGo4Status* stat=0;
   TGo4Task* task=fxDisplay->GetTask();
   while((stat=task->NextStatus(kFALSE))!=0) {
      //TGo4Log::Debug(" Display logger Timer displaying log...");
      fxDisplay->DisplayLog(stat);
   }
   Reset();
   return kFALSE;
}
