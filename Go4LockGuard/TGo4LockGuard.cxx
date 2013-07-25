// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4LockGuard.h"

#include "TMutex.h"

TMutex* TGo4LockGuard::fgxMainMutex=0;

Int_t TGo4LockGuard::fgiLockCount=0;


TGo4LockGuard::TGo4LockGuard (TMutex* mutex, Bool_t)
//: fbForceLock(forcelock)
{
   // first call: create main mutex
	//  std::cout <<"G-----TGo4LockGuard ctor" << std::endl;
	//
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

//#if ROOT_VERSION_CODE < ROOT_VERSION(5,31,0)
//if (fbForceLock || TThread::Exists()>0)
//#else
// JAM for the moment, we disable check for existing threads until problem with ROOT > 5.31 is solved
// suspect this is due to changed library linkage in root build
// note: was solved after ROOT version 5.32.00-rc1
//if(1)
//#endif

// NOTE2: since TThread::Exists() will use internal mutex lock anyway, we do not gain anything here
// this did save a lock in the old days only! JAM
   {
//         UInt_t id = TThread::SelfId();
         fxMutex->Lock();
         fbIsLocked=kTRUE;
         if(!fbIsMainMutex)
            {
//             if(TGo4Log::GetIgnoreLevel()>5)
//                {

            }
         else
            {
               fgiLockCount++;
               //if(TGo4Log::GetIgnoreLevel()>5)
//                   {
//                    std::cout <<"G-----{ Global Mutex "<< fxMutex << "acquired by thread id: "<<id;
//                    std::cout<< ", count:"<< fgiLockCount << std::endl;
//                   }
            }
      }
//   else
//      {
//         // no thread, no lock
//         fbIsLocked=kFALSE;
//      }
}

TGo4LockGuard::~TGo4LockGuard()
{
   if (fbIsLocked)
      {
//        UInt_t id = TThread::SelfId();
        if(!fbIsMainMutex)
           {
//              if(TGo4Log::GetIgnoreLevel()>5)
//                   {
//                    std::cout <<"}-----G Local Mutex "<< fxMutex << "released by thread id: "<<id << std::endl;
//                   }
           }
        else
            {
               fgiLockCount--;
//               if(TGo4Log::GetIgnoreLevel()>5)
//                   {
//                     std::cout <<"}-----G Global Mutex "<< fxMutex << "released by thread id: "<<id << std::endl;
//                     std::cout<< ", count:"<< fgiLockCount << std::endl;
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
