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

#ifndef TGO4RATEMETER_H
#define TGO4RATEMETER_H

#include "TNamed.h"

#include "TTimeStamp.h"

/**
 * Class containing event counter and ratemeter services.
 */
class TGo4Ratemeter : public TNamed {
   private:
      Bool_t       fbRunning{kFALSE};   // Indicates if analysis running
      ULong64_t    fuCurrentCount{0};    // Number of events processed since last start.
      Double_t     fdRate{0};            // Current eventrate (events/sec)
      Double_t     fdTime{0};            // Time sum since last ratemeter reset. in s.
      TString      fxDateString;         // Time and date of the last update of this status as sql string.
      TString      fxEventSource;        // Name of eventsource of first step / current filelist name

      ULong64_t    fuLastCount{0};       //! Number of events processed at last ratemeter update
      TTimeStamp   fLastTm;              //! Time when last rate measurement was done
      Bool_t       fbUpdateDone{kFALSE}; //! True if update has been performed since last TestZero call

      ULong64_t    fuNextCheckCnt{0};    //! Next counter value when time will be checked
      ULong64_t    fuCheckInterval{0};   //! How often time should be checked
      Double_t     fdUpdateInterval{0};  //! Configured interval in seconds before two updates

      static const Double_t fgdUPDATEINTERVAL;

   public:
      TGo4Ratemeter();

      virtual ~TGo4Ratemeter();

      Bool_t IsRunning() const { return fbRunning; }

      Double_t GetRate() const { return fdRate; }

      Double_t GetTime() const { return fdTime; }

      ULong64_t GetCurrentCount() const { return fuCurrentCount; }

      Double_t GetAvRate() const { return (fdTime > 0) ? ((Double_t) fuCurrentCount) / fdTime : 0.; }

      const char *GetDateTime() const { return fxDateString.Data(); }

      const char *GetCurrentSource() const { return fxEventSource.Data(); }

      void UpdateFrom(const TGo4Ratemeter* r);

      /** Update counter and rate values. Parameter increment may specify the
        * number of counts to add before the new rate is calculated.
        * Return kTRUE if new rate is calculated */
      Bool_t Update(Int_t increment = 1);

      void SetRunning(Bool_t on = kTRUE) { fbRunning = on; }
      void SetDateTime(const char *str) { fxDateString = str; }
      void SetCurrentSource(const char *str) { fxEventSource = str; }

      /** Reset counter and rate values */
      void Reset();

      /** Test if update has been called since last test.
        * To be called from watch thread to check if main thread is still active.
        * If not, watch thread has to update status and rate itself. */
      Bool_t TestUpdate();

      /** Set update interval in seconds - default is 1 */
      void SetUpdateInterval(double v) { if (v > 0) fdUpdateInterval = v; }

      ClassDefOverride(TGo4Ratemeter, 1)
};

#endif //TGO4RATEMETER_H
