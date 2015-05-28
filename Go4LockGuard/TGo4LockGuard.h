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

#ifndef TGO4LOCKGUARD_H
#define TGO4LOCKGUARD_H

/** Lockguard class to be used with go4 threads.
  * Usage: create a local instance of the lockguard in a certain scope
  * (e.g. at the begin of a function):
  * TGo4Lockguard mylockguard(fxMymutex);
  * Here fxMymutex is a user mutex which should be applied with this lock. If
  * TGo4Lockguard ctor is called without external mutex, it uses global Go4
  * mutex fxMainMutex.
  * Mutex is locked on creation of the TGo4Lockgurard, and unlocked on
  * deletion, i.e. whenever the scope is left (dtor will be called
  * automatically). This makes sure that mutex is released in any cases,
  * even on exception.
  * This class tests if root thread implementation is existing, so it will also
  * work without having any thread started. (root TLockguard class does not...)
  * @author J. Adamczewski
  * @author Go4 project-DVEE-GSI Darmstadt
  * @author Germany
  * @version 1.0
  * @since jul 2000-jun 2002*/

#include "Rtypes.h"

class TMutex;

class TGo4LockGuard {
   public:

      TGo4LockGuard(TMutex* mutex=0, Bool_t = kFALSE);

      virtual ~TGo4LockGuard();

      static void LockMainMutex();

      static void UnLockMainMutex();

      static Int_t MainMutexLockCount();

   private:

      /** Reference to Mutex associated with this lockguard (external or internal) */
      TMutex *fxMutex;        //!

      /** We need this flag if first TThread starts in between ctor and dtor. */
      Bool_t fbIsLocked;      //!

      /** True if the mutex of this lockguard instance is
        * the global go4 main mutex. */
      Bool_t fbIsMainMutex;   //!

      /** Main Mutex of the go4 system. Used to protect
        * any memory critical operations. This mutex is
        * applied if lockguard is called without giving
        * external mutex. */
      static TMutex* fgxMainMutex;   //!

      /** Counts the numbers of locks of the recursive main mutex. **/
      static Int_t fgiLockCount;     //!

   ClassDef(TGo4LockGuard,1)
};

#endif
