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

#ifndef TGo4SimpleSubEvent_H
#define TGo4SimpleSubEvent_H

#include "TGo4EventElement.h"

/**
 * @since 07-feb-2001
 * @author J. Adamczewski
 */
class TGo4SimpleSubEvent : public TGo4EventElement {
   public:
      /**
       * Processor ID [from setup]
       */
      Short_t fsProcid{0};

      /**
       * Length of the used data field (number of longwords). Corresponds to the
       * fiDlen Parameter of the MbsSubevent, which gives the fieldlength in shortwords.
       */
      Int_t fiFieldLen{0};

      /**
       * Data longword
       */
      Int_t fiD0{0};

      /**
       * Data longword
       */
      Int_t fiD1{0};

      /**
       * Data longword
       */
      Int_t fiD2{0};

      /**
       * Data longword
       */
      Int_t fiD3{0};

      /**
       * Data longword
       */
      Int_t fiD4{0};

      /**
       * Data longword
       */
      Int_t fiD5{0};

      /**
       * Data longword
       */
      Int_t fiD6{0};

      /**
       * Data longword
       */
      Int_t fiD7{0};

      /**
       * Data longword
       */
      Int_t fiD8{0};

      /**
       * Data longword
       */
      Int_t fiD9{0};

      /**
       * Data longword
       */
      Int_t fiD10{0};

      /**
       * Data longword
       */
      Int_t fiD11{0};

      /**
       * Data longword
       */
      Int_t fiD12{0};

      /** Method called by the event owner (analysis step) to fill the
        * event element from the set event source. Event source can
        * be the source of the analysis step (if this is a raw event) or the event processor (if this is a reduced event).
        * Fill method has to specify how the event source writes to the
        * members of this event structure. Either by calling methods of
        * the source (like myevent.a=GetEventSource()->GetA(); etc ), or by
        * direct copy to the member (like GetEventSource()->FillMemoryAt(&myevent.a))  */
      Int_t Fill() override;

      /**
       * Method called by the event owner (analysis step) to clear the
       * event element.
       */
      void Clear(Option_t *opt = "") override;

      void PrintEvent() override;

      TGo4SimpleSubEvent();

      virtual ~TGo4SimpleSubEvent();

   ClassDefOverride(TGo4SimpleSubEvent,1)
};

#endif
