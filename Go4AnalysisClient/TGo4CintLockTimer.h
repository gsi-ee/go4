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

#ifndef TGO4CINTLOCKTIMER_h
#define TGO4CINTLOCKTIMER_h

#include "TTimer.h"

class TGo4ThreadManager;
class TCondition;
class TGo4AnalysisClient;

/**
*   This timer is used to protect the Go4 threads against
*   the CINT main application when go4 is running as cint
*   server. Go4 thread activities that acquire the Go4 main mutex
*   are only allowed within a small time window inside the
*   Notify of this timer. Besides this, the root application will
*   hold the main mutex completely, thus avoiding go4 threads
*   to interfere with root system. Note: this mechanism is the
*   complement to the threadmanager application blocking mode,
*   meant to block the root application and allow the threads
*   everything.
* @author J. Adamczewski
* @since  12/5/2005
*/

class TGo4CintLockTimer : public TTimer
{

  public:

      TGo4CintLockTimer (TGo4AnalysisClient* parent, Long_t msperiod = 3000, Bool_t mode = kTRUE);

      virtual ~TGo4CintLockTimer();

     /** Routine called from TTimer on timeout */
      virtual Bool_t Notify ();


  private:
    /**
      *   Time window in ms between unlock of mutex and
      *   the next request in this timer notify.
      *   May be used for tuning application performance.
      **/
      static UInt_t fguTHREADTIME;

      /** True if this timer has locked the main mutex */
      Bool_t fbHaveLock;

      TGo4AnalysisClient* fxParent;//!
};

#endif
