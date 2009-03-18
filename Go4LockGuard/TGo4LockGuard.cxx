#include "TGo4LockGuard.h"

#include "Riostream.h"
#include "TMutex.h"

TMutex* TGo4LockGuard::fgxMainMutex=0;

Int_t TGo4LockGuard::fgiLockCount=0;


TGo4LockGuard::TGo4LockGuard (TMutex* mutex)
{
   // first call: create main mutex
   if(fgxMainMutex==0)
      fgxMainMutex= new TMutex(kTRUE); // we use recursive mode for cascading lockguards
   if(mutex==0)
      {
         // use global mutex
         fxMutex=fgxMainMutex;
         fbIsMainMutex=kTRUE;
      }
   else
      {
         // use external mutex
         fxMutex=mutex;
         fbIsMainMutex=kFALSE;
      }

   if (TThread::Exists()>0)
      {
         // UInt_t id = TThread::SelfId();
         fxMutex->Lock();
         fbIsLocked=kTRUE;
         if(!fbIsMainMutex)
            {
//             if(TGo4Log::GetIgnoreLevel()>5)
//                {
//                  cout <<"G-----{ Local Mutex "<< fxMutex << " acquired by thread id: "<<id << endl;
//                }
            }
         else
            {
               fgiLockCount++;
               //if(TGo4Log::GetIgnoreLevel()>5)
//                   {
//                     cout <<"G-----{ Global Mutex "<< fxMutex << "acquired by thread id: "<<id;
//                     cout<< ", count:"<< fgiLockCount << endl;
//                   }
            }
      }
   else
      {
         // no thread, no lock
         fbIsLocked=kFALSE;
      }
}

TGo4LockGuard::~TGo4LockGuard()
{
   if (fbIsLocked)
      {
        //UInt_t id = TThread::SelfId();
        if(!fbIsMainMutex)
           {
//              if(TGo4Log::GetIgnoreLevel()>5)
//                   {
//                     cout <<"}-----G Local Mutex "<< fxMutex << "released by thread id: "<<id << endl;
//                   }
           }
        else
            {
               fgiLockCount--;
//               if(TGo4Log::GetIgnoreLevel()>5)
//                   {
//                     cout <<"}-----G Global Mutex "<< fxMutex << "released by thread id: "<<id << endl;
//                     cout<< ", count:"<< fgiLockCount << endl;
//                   }
            }
         fxMutex->UnLock();
      }
   else
      {
         // no woman, no cry
      }
}

void TGo4LockGuard::LockMainMutex()
{
  fgxMainMutex->Lock();
}

void TGo4LockGuard::UnLockMainMutex()
{
  fgxMainMutex->UnLock();
}
