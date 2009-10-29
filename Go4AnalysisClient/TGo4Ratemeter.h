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

#ifndef TGO4RATEMETER_H
#define TGO4RATEMETER_H

#include "TObject.h"

class TStopwatch;

/**
 * Class containing event counter and ratemeter services.
 */
class TGo4Ratemeter : public TObject {
   public:
      static const Double_t fgdUPDATEINTERVAL;
      TGo4Ratemeter() ;

      virtual ~TGo4Ratemeter() ;

      Double_t GetRate() const { return fdRate; }

      Double_t GetAvRate() const { return fdAveRate; }

      Double_t GetTime() const { return fdTime; }

      UInt_t GetCurrentCount() const { return fuCurrentCount; }

      /** Update counter and rate values. Parameter increment may specify the
        * number of counts to add before the new rate is calculated. */
      void Update(Int_t increment=1);

      /** Update counter and rate values. Parameter increment may specify the
        * number of counts to add before the new rate is calculated. */
      void Reset();

      /** Test if update has been called since last test..
        * To be called from watch thread to check if main thread is still active.
        * If not, watch thread has to update status and rate itself. */
      Bool_t TestUpdate();

   private:

      /** Number of events processed since last start. */
      UInt_t fuCurrentCount;

      /** Number of events processed at last ratemeter update. */
      UInt_t fuLastCount;

      /** Current eventrate (events/sec). */
      Double_t fdRate;

      /** Average eventrate since last reset (events/sec). */
      Double_t fdAveRate;

      /** Time sum since last ratemeter reset. in s. */
      Double_t fdTime;

      /** Time sum at last ratemeter update. in s. */
      Double_t fdLastTime;

      /** Time interval since last Rate Calculation. In s. */
      Double_t fdDeltaTime;

      TStopwatch* fxClock; //!

      /** True if update has been performed since last TestZero call. */
      Bool_t fbUpdateDone;
};

#endif //TGO4RATEMETER_H
