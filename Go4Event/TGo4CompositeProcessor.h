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

#ifndef TGO4COMPOSITEPROCESSOR_H
#define TGO4COMPOSITEPROCESSOR_H

#include "TGo4EventProcessor.h"

#include "TObjArray.h"

class TGo4MbsEvent;
class TGo4MbsSubEvent;
class TGo4CompositeEvent;

/** @brief Composite processor
 * @ingroup go4_event
 */
class TGo4CompositeProcessor : public TGo4EventProcessor {
   public:

      TGo4CompositeProcessor();

      TGo4CompositeProcessor(const char *name);

      virtual ~TGo4CompositeProcessor();

      /* Can be overwritten by subclass, but is not recommended! use ProcessEvent or ProcessSubevent instead*/
      Bool_t BuildEvent(TGo4EventElement *) override;

      /* implemented by subclass to assign correct event structures*/
      void InitEvent(TGo4EventElement *) override {}

      /* implemented by subclass to do actual processing. For second step processors!*/
      void FinalizeEvent() override {}

      /* implemented by subclass to process incoming subevent. For first step processors only! */
      virtual void ProcessSubevent(TGo4MbsSubEvent *subevt);

      /** Returns trigger number of main MBS event */
      Short_t GetTriggerNumber() const { return fMbsTriggerNumber; }

      /** Returns event number of main MBS event */
      Int_t GetEventNumber() const { return fMbsEventNumber; }

   protected:

      void AddSubProcessor(TGo4EventProcessor *);

      TGo4MbsEvent        *fMbsInput{nullptr}; //!
      Short_t              fMbsTriggerNumber{0};
      Int_t                fMbsEventNumber{0};

   private:
      TObjArray            fSubProcessors; //!

      ClassDefOverride(TGo4CompositeProcessor,1)
};

#endif

