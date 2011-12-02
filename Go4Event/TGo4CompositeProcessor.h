// $Id: TGo4CompositeEvent.h 815 2011-12-02 16:34:08Z linev $
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

#ifndef TGO4COMPOSITEPROCESSOR_H
#define TGO4COMPOSITEPROCESSOR_H

#include "TGo4EventProcessor.h"

#include "TObjArray.h"

class TGo4MbsEvent;
class TGo4MbsSubEvent;
class TGo4CompositeEvent;
class TIterator;

class TGo4CompositeProcessor : public TGo4EventProcessor {
   public:

      TGo4CompositeProcessor();

      TGo4CompositeProcessor(const char* name);

      virtual ~TGo4CompositeProcessor();

      /* Can be overwritten by subclass, but is not recommended! use ProcessEvent or ProcessSubevent instead*/
      virtual Bool_t BuildEvent(TGo4EventElement*);

      /* implemented by subclass to assign correct event structures*/
      virtual void InitEvent(TGo4EventElement*) {}

      /* implemented by subclass to do actual processing. For second step processors!*/
      virtual void FinalizeEvent() {}

      /* implemented by subclass to process incoming subevent. For first step processors only! */
      virtual void ProcessSubevent(TGo4MbsSubEvent* subevt);

      /** Returns trigger number of main MBS event */
      Short_t GetTriggerNumber() const { return fMbsTriggerNumber; }

      /** Returns event number of main MBS event */
      Int_t GetEventNumber() const { return fMbsEventNumber; }

   protected:

      void AddSubProcessor(TGo4EventProcessor* );

      TGo4CompositeEvent  *fBeamtimeEvent; //!
      TGo4MbsEvent        *fMbsInput; //!
      Short_t              fMbsTriggerNumber;
      Int_t                fMbsEventNumber;

   private:
      TObjArray   fSubProcessors; //!

      ClassDef(TGo4CompositeProcessor,1)
};

#endif

