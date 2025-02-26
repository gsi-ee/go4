// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGo4Thread_H
#define TGo4Thread_H

#include "TNamed.h"

class TThread;
class TCondition;
class TGo4Runnable;

/** @brief go4 thread class
 * @details Class that encapsulates a root TThread which calls a user runnable Method
  *   (TGo4Runnable::Run()) in a loop. This working loop can be suspended to a
  *   condition wait to stop the action.
  * @ingroup go4_threadmgr
  * @author J. Adamczewski(at)gsi.de
  * @author Go4 project-DVEE-GSI Darmstadt
  * @author Germany
  * @version 0.9
  * @since jul 2000-oct 2001  */

class TGo4Thread : public TNamed {

   public:

      TGo4Thread() = delete;

      TGo4Thread(const TGo4Thread &right);

      /** TGo4Thread constructor parameters:
        *   name:       name of the thread
        *   runnable:    external user runnable to be executed from TThread.
        *   internal:    define if thread shall be owned by threadmanager,
        *            or if it belongs to other aggregation */
      TGo4Thread (const char *name, TGo4Runnable *runnable, Bool_t internal = kTRUE);

      virtual ~TGo4Thread();

      /**  creates the TThread if not yet existing */
      Bool_t Create();

      /**  Cancels TThread */
      Bool_t Cancel();

      /**   Stops old Runnable;
        *   Creates new TThread of same Runnable and name;
        *   cancels old Thread */
      Bool_t ReCreate();

      /**  starts runnable method Run within thread loop
        *  by setting running flag and signaling condition */
      Bool_t Start();

      /** resets running flag for runnable */
      Bool_t Stop();

      /** wrapper for gSystem->Sleep with consecutive
        * TThread::CancelPoint - necessary for proper pthread termination */
      static void Sleep(UInt_t millisecs);

      /** linux process id associated with the pthread */
      Int_t GetPID() const { return fiThreadPID; }

      /** self id of the current thread */
      Long_t GetSelfID() const { return fiThreadSelfID; }

      /** returns flag indicating if go4thread object is internal
        * to threadhandler or created externally */
      Bool_t IsInternal() const { return fbIsInternal; }

      /** kTRUE if TThread instance exists and pthread is created */
      Bool_t IsCreated() const { return fbIsCreated; }

      /** Flag that controls Workfunc loop within Threadfunc */
      Bool_t IsRunning() const { return fbIsRunning; }

      /** true if Threadfunc is suspended to condition wait */
      Bool_t IsWaiting() const { return fbIsWaiting; }

   private:

      /**  This function is passed to TThread ctor and runs as pthread.
        *  Contains a loop that is controlled by Go4Thread own TCondition and
        *  by "function shall run" flag.
        *  Within the loop the virtual working function is called. */
      static void Threadfunc (void *arg);

      /** We need this to have access to instance runnable from static Threadfunc: */
      TGo4Runnable *GetRunnable() const { return fxRunnable; }

      TCondition *GetCondition() const { return fxCondition; }

      void SetPID();

      void SetSelfID();

      void SetWaiting(Bool_t mode=kTRUE) { fbIsWaiting=mode; }

      /** specifies whether thread is internal thread of handler
        *   (true) or is owned by external class (false).
        * In the latter case,
        *   thread instance and its runnable  are not deleted when
        *   it is removed from the thread list */
      Bool_t fbIsInternal{kFALSE};

      /** kTRUE if TThread instance exists and pthread is created */
      Bool_t fbIsCreated{kFALSE};

      /** Flag that controls Workfunc loop within Threadfunc */
      Bool_t fbIsRunning{kFALSE};

      /** Linux process id associated with the pthread */
      Int_t fiThreadPID{0};

      /** self id of the current thread */
      Long_t fiThreadSelfID{0};

      /** true if Threadfunc is suspended to condition wait */
      Bool_t fbIsWaiting{kFALSE};

      /**  External class providing the threaded workfunction
        *   and exception routines for workfunction. The threaded
        *   action will be defined by creating a runnable subclass
        *   and overriding the TGo4Runnable::Run() method. Here exception
        * handling behaviour may be overridden, too.
        * @clientCardinality 1
        * @supplierCardinality 1 */
      TGo4Runnable *fxRunnable{nullptr}; //!

      TThread *fxThread{nullptr};  //!

      TCondition *fxCondition{nullptr};  //!
};

#endif
