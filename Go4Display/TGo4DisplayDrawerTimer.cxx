#include "TGo4DisplayDrawerTimer.h"

#include "TGo4Log.h"
#include "TGo4Task.h"
#include "TGo4Display.h"

TGo4DisplayDrawerTimer::TGo4DisplayDrawerTimer (TGo4Display* display, Long_t msperiod)
   :TTimer(msperiod,kTRUE),fxDisplay(display)
{
   TGo4Log::Debug(" Display drawing Timer is started with %d ms period.", msperiod);
}

TGo4DisplayDrawerTimer::~TGo4DisplayDrawerTimer()
{
}

Bool_t TGo4DisplayDrawerTimer::Notify ()
{
   TObject* data=0;
   TGo4Task* task=fxDisplay->GetTask();

//   data = task->NextObject(kFALSE);
//   if (data!=0)
//      fxDisplay->DisplayData(data);

   Int_t cnt = 100;

   while((data=task->NextObject(kFALSE))!=0) {
      fxDisplay->DisplayData(data);
      if (cnt--<0) break;
   }

   Reset();
   return kFALSE;
}
