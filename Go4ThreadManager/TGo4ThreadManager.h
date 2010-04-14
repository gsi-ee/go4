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

#ifndef TGo4ThreadManager_H
#define TGo4ThreadManager_H

#include "TNamed.h"

class TGo4ThreadHandler;
class TGo4AppControlTimer;

/** Base class for classes that
  *   shall be accessed from a thread /runnable instance
  * @author J. Adamczewski
  * @author Go4 project-DVEE-GSI Darmstadt
  * @author Germany
  * @author j.adamczewski@gsi.de
  * @version 0.9
  * @since jul 2000-oct 2001  */

class TGo4ThreadManager : public TNamed {

   public:
      TGo4ThreadManager(const TGo4ThreadManager &right);

     /** ctor parameters:
      *   blockingmode=true: gApplication is blocked by timer
      *   autostart=true: all thread runnables start their work on Initialization()
      *   autocreate=true: all threads added to handler in threadmanager ctor are created on Launch() **/
      TGo4ThreadManager (const char* name, Bool_t blockingmode = kTRUE, Bool_t autostart = kTRUE, Bool_t autocreate = kTRUE);

      virtual ~TGo4ThreadManager();

      /** user can exchange the standard control timer
        * by an own derived one by this method **/
      void SetTimer(TGo4AppControlTimer* ctrltimer) { fxBlocker=ctrltimer; }

      /** Access to ThreadHandler for working threads */
      TGo4ThreadHandler* GetWorkHandler() const { return fxWorkHandler; }

      /** Blocks gSystem by means of the blocking timer */
      Bool_t BlockApp();

     /** Wakes up the control timer waiting for his condition;
       * parameter mode specifies whether timer will block
       * again at next turn (mode=0), timer will block when
       * all threads are up (mode=1), or timer will not block
       * again unless the next Block call, i.e. applicationrun flag is
       * reset (mode=2) */
      Bool_t UnBlockApp(Int_t mode = 0);

      virtual Int_t Initialization();

      Bool_t IsInitDone() const { return fbInitDone; }

      /** returns termination status of Threadmanager */
      Bool_t IsTerminating () const { return fbTerminating; }

      Bool_t IsTerminateApp() const { return fbTerminateApplication; }

      /** Returns true if threadmanager works in root system blocking mode **/
      Bool_t IsAppBlocking() const { return fbAppBlocking; }


      /** Returns true if threadmanager works in root system blocking mode **/
      Bool_t IsBeingQuit() const { return fbBeingQuit; }

      void SetBeingQuit(Bool_t on) {fbBeingQuit=on;}


      /** This Method has to be called to create the startup threads and to
        * turn on the application control timer.
        * Usually this is done at the end of the user's threadmanager constructor.
        * The idea is that no thread should be started unless the user initializations
        * are done; therefore, its up to the user to define when to Launch. **/
      void Launch ();

      /** deletes the Manager instance via Control timer.
        *   If termap is true, also
        *   terminates the root Application **/
      virtual void Terminate (Bool_t termapp=kTRUE);

      /** Alternative termination in case of error */
      virtual void TerminateFast();

      /** Period in ms for the application control timer */
      static const Long_t fglTIMERPERIOD;

      /** Default textlength for char arrays **/
      enum { fguTEXTLENGTH = 256 };

   protected:

      /** is set kTRUE after first Initialize is done */
      Bool_t fbInitDone;

      /** flag showing termination status */
      Bool_t fbTerminating;

      /** If true, terminate complete application. If false,
        * only terminate threadmanager instance. */
      Bool_t fbTerminateApplication;

      /** true if threadmanager shall block root gSystem using
        * the AppControlTimer; false if gSystem shall never be
        * blocked (necessary for gui) */
      Bool_t fbAppBlocking;

      /** If true, TThreads of all go4threads in list shall be created automatically
        * on calling Launch(). */
      Bool_t fbAutoCreate;

      /** If true, workfunctions (runnable Run()) of all go4threads in list shall be
        * started automatically on Initialization. If false, TThreads may be running, but
        * workfunctions suspend in condition wait. */
      Bool_t fbAutoStart;

      /* flag that can be set at begin of application quit sequence.
       * Can be optionally used by application to implement a quit timeout
       * that will terminate if regular shutdown hangs up*/
      Int_t fbBeingQuit;

      /** Composition of Application control timer.
        * @link aggregationByValue
        * @clientCardinality 1
        * @supplierCardinality 1 */
      TGo4AppControlTimer *fxBlocker; //!

      /** Thread handler aggregate (threadsafe list of go4 threads)
        * @link aggregationByValue
        * @clientCardinality 1
        * @supplierCardinality 1 */
      TGo4ThreadHandler *fxWorkHandler;  //!

  private:
      TGo4ThreadManager();

   ClassDef(TGo4ThreadManager,1)
};

#endif
