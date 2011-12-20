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

#include "TTimeStamp.h"

/**
 * Class containing event counter and ratemeter services.
 */
class TGo4Ratemeter : public TObject {
   public:
      TGo4Ratemeter() ;

      virtual ~TGo4Ratemeter() ;

      Double_t GetRate() const { return fdRate; }

      Double_t GetTime() const { return fdTime; }

      ULong64_t GetCurrentCount() const { return fuCurrentCount; }

      Double_t GetAvRate() const { return fdTime>0 ? fuCurrentCount/fdTime : 0.; }

      /** Update counter and rate values. Parameter increment may specify the
        * number of counts to add before the new rate is calculated.
        * Return kTRUE if new rate is calculated */
      Bool_t Update(Int_t increment=1);

      /** Reset counter and rate values */
      void Reset();

      /** Test if update has been called since last test..
        * To be called from watch thread to check if main thread is still active.
        * If not, watch thread has to update status and rate itself. */
      Bool_t TestUpdate();

      /** Set update interval in seconds - default is 1 */

      void SetUpdateInterval(double v) { if (v>0) fdUpdateInterval = v; }

   private:

      static const Double_t fgdUPDATEINTERVAL;

      /** Number of events processed since last start. */
      ULong64_t fuCurrentCount;

      /** Number of events processed at last ratemeter update. */
      ULong64_t fuLastCount;

      /** Current eventrate (events/sec). */
      Double_t fdRate;

      /** Time sum since last ratemeter reset. in s. */
      Double_t fdTime;

      /** Time when last rate measurement was done */
      TTimeStamp fLastTm;

      /** True if update has been performed since last TestZero call. */
      Bool_t fbUpdateDone;

      /** Next counter value when time will be checked */
      ULong64_t fuNextCheckCnt;

      /** How often time should be checked */
      ULong64_t fuCheckInterval;

      /** Configured interval in seconds before two updates */
      Double_t fdUpdateInterval;
};

#endif //TGO4RATEMETER_H
