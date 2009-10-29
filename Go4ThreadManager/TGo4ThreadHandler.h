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

#ifndef TGo4ThreadHandler_H
#define TGo4ThreadHandler_H

#include "TNamed.h"

class TGo4Thread;
class TIterator;
class TObjArray;
class TMutex;
class TGo4ThreadManager;
class TGo4Runnable;

/** Thread Handler holds array of Go4Thread Objects; each
  *   Go4Thread handles one TThread and Mutex, Condition...
  *   knows threadmanager instance
  *   that owns the handler (that owns the
  *   task connector  which owns the thread handler, respectively)
  *   this threadmanager instance pointer is given to each thread and runnable on
  *   creation, thus each thread can acces its manager  methods by
  *   threadmanager pointer
  * @author J. Adamczewski
  * @author Go4 project-DVEE-GSI Darmstadt
  * @author Germany
  * @author j.adamczewski@gsi.de
  * @version 0.9
  * @since jul 2000-oct 2001  */

class TGo4ThreadHandler : public TNamed {

   public:
      TGo4ThreadHandler(const TGo4ThreadHandler &right);

      TGo4ThreadHandler (const char* name, TGo4ThreadManager* parent);

      virtual ~TGo4ThreadHandler();

      /** Adds Reference to (externally created) thread to Thread List **/
      Bool_t AddThread (TGo4Thread* gthr);

      /** Remove thread by name from Thread List */
      Bool_t RemoveThread (const char* name);

      /** Creates new internal Go4 Thread with name and adds it to the List
        * External runnable is passed to specify the threaded actions **/
      Bool_t NewThread(const char* name, TGo4Runnable* runnable);

      /** Launch all Threads in the thread list. */
      Int_t CreateAll();

      /** Launch the ROOT Thread specified by the Go4 Thread of name 'name'.
        * Name of TThread and TGo4Thread is the same. */
      Bool_t Create(const char* thname);

      /** Cancel all Threads in the thread list. */
      Int_t CancelAll();

      /** Cancels the ROOT Thread specified by the Go4 Thread of name 'name'.
        * Name of TThread and TGo4Thread is the same. */
      Bool_t Cancel(const char* thname);

      /** Recreate all Threads in the thread list. */
      Int_t ReCreateAll();

      /** ReCreate (Cancel and Create) all Threads in the thread list. **/
      Bool_t ReCreate(const char* thname);

      /** Start work function of all Threads in the thread list. **/
      Int_t StartAll();

      /** Starts work function of the Go4 Thread of name 'name'.
        * Workfunction will run in infinite loop until Stop(). **/
      Bool_t Start(const char* thname);

      /** Start work function of all Threads in the thread list. **/
      Int_t StopAll ();

      /** Stops work function of the Go4 Thread of name 'name'.
        * Thread will suspend to condition wait without being killed.
        * Workfunction will continue loop on next Sart().**/
      Bool_t Stop(const char* thname);

      /**  dumps thread process information to logging output
        *   i.e. file or anything **/
      Int_t DumpThreads(Int_t mode = 0);

      /** Access to Go4Thread by name. */
      TGo4Thread* GetThread(const char* name);

      /** Number of threads in list **/
      Int_t GetEntries();

      /** Returns true if all root threads in list are existing,
        * i.e. there is a pid and a valid thread pointer for them
        *   in go4 thread **/
      Bool_t AllCreated();

      /** Returns true if all threads in list are running, i.e.
        * all workfunctions were started */
      Bool_t AllRunning();

      /** Returns true if all threads in list are waiting, i.e.
        * all workfunctions are in condition wait */
      Bool_t AllWaiting();

      /** Returns kTRUE if threadhandler performs any action that
        * requires gSystem support. Flag is checked by Manager
        * before Blocking the System. **/
      Bool_t IsOperating() const { return fbIsOperating; }

   protected:

      /** Backlink to threadmanager who owns this thread handler.
        * @supplierCardinality 1
        * @clientCardinality 1 */
      TGo4ThreadManager *fxManager;

   private:

      /** kTRUE, if threadhandler performs any action that
        * requires gSystem support. Flag is checked by Manager
        * before Blocking the System using IsOperating() method **/
      Bool_t fbIsOperating;

      /** Mutex protecting thread array */
      TMutex *fxListMutex;

      /** Iterator for Thread Array */
      TIterator *fxIterator;

      /** Array of Go4Threads */
      TObjArray *fxArray;

      /** Mutex to protect IsOperating flag, flag is set kTRUE to
        * indicate that AppControl timer must not block, since
        * thread launching or canceling operation is performed by
        * thread handler. This state is checked by threadmanager in
        * BlockApp() method before enabling timer wait.
        * Mechanism should prevent timer deadlocks. */
      TMutex *fxOperMutex;

      TGo4ThreadHandler();
};

#endif
