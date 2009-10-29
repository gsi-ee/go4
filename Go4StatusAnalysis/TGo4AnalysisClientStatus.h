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

#ifndef TGO4ANALYSISCLIENTSTATUS_H
#define TGO4ANALYSISCLIENTSTATUS_H

#include "TGo4ClientStatus.h"

#include "TString.h"

class TGo4AnalysisClientStatus : public TGo4ClientStatus {

   friend class TGo4AnalysisClient;

   public:

      TGo4AnalysisClientStatus();

      TGo4AnalysisClientStatus(const char* name);

      virtual ~TGo4AnalysisClientStatus() ;

      Double_t GetRate() const { return fdRate; }

      Double_t GetAvRate() const { return fdAveRate; }

      Double_t GetTime() const { return fdTime; }

      UInt_t GetCurrentCount() const { return fuCurrentCount; }

      const char* GetDateTime() const { return fxDateString.Data(); }

      const char* GetCurrentSource() const { return fxEventSource.Data(); }

      Bool_t IsAnalysisRunning(){return fbAnalysisRunning;}

      /** basic method to printout status information
       * on stdout; to be overridden by specific subclass */
      virtual Int_t PrintStatus(Text_t* buffer=0, Int_t buflen=0);

   private:
      void SetRates(Double_t rate, Double_t average, UInt_t counts, Double_t time);

      void SetCurrentSource(const char* name) { fxEventSource = name; }

      void SetRunning(Bool_t on) { fbAnalysisRunning = on; }

      /** Number of events processed since last start. */
      UInt_t fuCurrentCount;

      /** Current eventrate (events/sec). */
      Double_t fdRate;

      /** Average eventrate since last reset (events/sec). */
      Double_t fdAveRate;

      /** Time sum since last reset. in s. */
      Double_t fdTime;

      /** Time and date of the last update of this status as sql string. */
      TString fxDateString;

      /** Name of eventsource of first step / current filelist name */
      TString fxEventSource;

      /** True running state of analysis, independent of event rate*/
      Bool_t fbAnalysisRunning;

      ClassDef(TGo4AnalysisClientStatus,1)
};

#endif //TGO4ANALYSISCLIENTSTATUS_H
