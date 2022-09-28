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

#ifndef TGO4ANALYSISCLIENTSTATUS_H
#define TGO4ANALYSISCLIENTSTATUS_H

#include "TGo4ClientStatus.h"

#include "TString.h"

class TGo4AnalysisClientStatus : public TGo4ClientStatus {

   friend class TGo4AnalysisClient;

   public:

      TGo4AnalysisClientStatus();

      TGo4AnalysisClientStatus(const char *name);

      virtual ~TGo4AnalysisClientStatus();

      Double_t GetRate() const { return fdRate; }

      Double_t GetAvRate() const { return fdAveRate; }

      Double_t GetTime() const { return fdTime; }

      ULong64_t GetCurrentCount() const { return fuCurrentCount; }

      const char* GetDateTime() const { return fxDateString.Data(); }

      const char* GetCurrentSource() const { return fxEventSource.Data(); }

      Bool_t IsAnalysisRunning() const { return fbAnalysisRunning; }

      void Print(Option_t* = "") const override;

   private:
      void SetRates(Double_t rate, Double_t average, ULong64_t counts, Double_t time);

      void SetCurrentSource(const char *name) { fxEventSource = name; }

      void SetRunning(Bool_t on) { fbAnalysisRunning = on; }

      /** Number of events processed since last start. */
      ULong64_t fuCurrentCount{0};

      /** Current eventrate (events/sec). */
      Double_t fdRate{0};

      /** Average eventrate since last reset (events/sec). */
      Double_t fdAveRate{0};

      /** Time sum since last reset. in s. */
      Double_t fdTime{0};

      /** Time and date of the last update of this status as sql string. */
      TString fxDateString;

      /** Name of eventsource of first step / current filelist name */
      TString fxEventSource;

      /** True running state of analysis, independent of event rate*/
      Bool_t fbAnalysisRunning{kFALSE};

      ClassDefOverride(TGo4AnalysisClientStatus,2)
};

#endif //TGO4ANALYSISCLIENTSTATUS_H
