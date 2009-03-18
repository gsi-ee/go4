#include "TGo4CintLockTimer.h"

#include "Riostream.h"

#include "TThread.h"
#include "TApplication.h"

#include "TGo4Log.h"
#include "TGo4AnalysisClient.h"

UInt_t TGo4CintLockTimer::fguTHREADTIME = 50;//50

TGo4CintLockTimer::TGo4CintLockTimer (TGo4AnalysisClient* parent,Long_t msperiod, Bool_t mode)
   :TTimer(msperiod,mode),fbHaveLock(kFALSE),fxParent(parent)
{
}

TGo4CintLockTimer::~TGo4CintLockTimer()
{
if(fbHaveLock) fxParent->UnLockAll();
}

Bool_t TGo4CintLockTimer::Notify()
{
//TurnOff();
if(fbHaveLock)
   {
      //if(TThread::Exists()) TGo4LockGuard::UnLockMainMutex();
      fxParent->UnLockAll();
      //cout <<"U TGo4CintLockTimer unlocked" << endl;
      fbHaveLock=kFALSE;
   }

gSystem->Sleep(fguTHREADTIME);
if(!fbHaveLock)
   {
      //if(TThread::Exists()) TGo4LockGuard::LockMainMutex();
      fxParent->LockAll();
      //cout <<"L TGo4CintLockTimer has lock" << endl;
      fbHaveLock=kTRUE;
   }
Reset();
//TurnOn();
return kFALSE;
}





