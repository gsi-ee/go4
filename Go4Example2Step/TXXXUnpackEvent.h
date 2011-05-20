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

#ifndef TXXXEVENT_H
#define TXXXEVENT_H

#define XXX_NUM_CHAN 8

#include "TGo4EventElement.h"

class TXXXUnpackEvent : public TGo4EventElement {
   public:
      TXXXUnpackEvent() : TGo4EventElement() {}
      TXXXUnpackEvent(const char* name) : TGo4EventElement(name) {}
      virtual ~TXXXUnpackEvent() {}

      /**
       * Method called by the event owner (analysis step) to clear the
       * event element.
       */
      virtual void Clear(Option_t *t="");

      Int_t fiCrate1[XXX_NUM_CHAN];
      Int_t fiCrate2[XXX_NUM_CHAN];
      Int_t fiCrate3[XXX_NUM_CHAN];
      Int_t fiCrate4[XXX_NUM_CHAN];

   ClassDef(TXXXUnpackEvent,1)
};
#endif //TXXXEVENT_H



