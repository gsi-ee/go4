#ifndef TGo4AppControlTimer_h
#define TGo4AppControlTimer_h

#include "TTimer.h"

class TGo4ThreadManager;
class TCondition;

/**
*   Timer with main purpose to block the root
*   TApplication loop during thread execution. Moreover,
*   this timer calls Initialization() function of threadmanager,
*   which can be overridden by user to define any actions which should not
*   be run within a thread (critical root system calls, like opening TServerSocket,
*   should be done within the application loop, i.e. in the timer).
*   In threadmanager non-blocking-mode, Initialization() is called from timer
*   regularly with the period specified in threadmanager; in blocking mode
*   it is called once after each threadmanager UnBlockApp().
*   This timer is also responsible for proper termination of the threadmanager.
* @author J. Adamczewski
* @author Go4 project-DVEE-GSI Darmstadt
* @author Germany
* @author j.adamczewski@gsi.de
* @version 0.9
* @since jul 2000-oct 2001*/

class TGo4AppControlTimer : public TTimer {

  public:
      TGo4AppControlTimer (TGo4ThreadManager* manager, Long_t msperiod = 3000, Bool_t mode = kTRUE);

      virtual ~TGo4AppControlTimer();

     /** Sets reference to new thread manager */
      void SetManager (TGo4ThreadManager* parent) { fxManager=parent; }

     /** Routine called from TTimer on timeout */
      virtual Bool_t Notify ();

      /**
       *   Returns handle for internal condition which is
       *   used to block the gSystem.
       **/
      const TCondition* GetCondition () const { return fxCondition;}

     /**
      *   Switches between blocking and non blocking mode of the timer.
      *   If set to kTRUE, timer will not block the root system. If set
      * to kFALSE, timer will suspend in a condition wait and therefore the
      * InnerLoop() of root is blocked.
      **/
      void SetApplicationRun (Bool_t flag = kTRUE) { fbApplicationRun=flag; }

     /**
      *   Status of the application run flag. If true, root application is _not_ blocked.
      **/
      Bool_t GetApplicationRun();

  protected:
    /**
      *   time in ms to wait for all Runnables being stopped
      *   before Termination; after this time, the AllWaiting()
      *   of TGo4ThreadHandler is tested again, until the
      *   max number of testing cycles (fgiTERMWAITCYCLES) is
      *   reached; then Termination is done anyway
      **/
      static UInt_t fguTERMWAIT;       //!
    /**
      *   On Termination, Notify will test fgiTERMWAITCYCLES
      *   times if Runnables are stopped, i.e. all TGo4Threads
      *   are in TCondition wait (safe cancelling point);
      *   afterwards,
      *   threads are deleted anyway
      **/
      static Int_t fgiTERMWAITCYCLES;     //!

      /**
       * Backlink to threadmanager who owns this timer.
       * @clientCardinality 1
       * @supplierCardinality 1
       */
      TGo4ThreadManager *fxManager;     //!

  private:

      Bool_t fbApplicationRun;           //!

      TCondition *fxCondition;          //!

      TGo4AppControlTimer();
};

#endif
