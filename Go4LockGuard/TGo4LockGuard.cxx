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

TMutex* TGo4LockGuard::fgxMainMutex = 0;

Int_t TGo4LockGuard::fgiLockCount = 0;


TGo4LockGuard::TGo4LockGuard (TMutex* mutex, Bool_t)
{
   // first call: create main mutex
	//  std::cout <<"G-----TGo4LockGuard ctor" << std::endl;
	//
   if(fgxMainMutex==0)
      fgxMainMutex = new TMutex(kTRUE); // we use recursive mode for cascading lockguards
   if(mutex==0) {
      // use global mutex
      fxMutex = fgxMainMutex;
      fbIsMainMutex = kTRUE;
   } else {
      // use external mutex
      fxMutex = mutex;
      fbIsMainMutex = kFALSE;
   }

   fxMutex->Lock();
   fbIsLocked = kTRUE;
   if(fbIsMainMutex) fgiLockCount++;
}

TGo4LockGuard::~TGo4LockGuard()
{
   if (fbIsLocked) {
        if(fbIsMainMutex) fgiLockCount--;
        fxMutex->UnLock();
   }
}

Int_t TGo4LockGuard::MainMutexLockCount()
{
   return fgiLockCount;
}

void TGo4LockGuard::LockMainMutex()
{
  fgxMainMutex->Lock();
}

void TGo4LockGuard::UnLockMainMutex()
{
  fgxMainMutex->UnLock();
}
