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

#ifndef TXXXANLEVENT_H
#define TXXXANLEVENT_H

#include "TGo4EventElement.h"
#include "TXXXUnpackEvent.h"

class TXXXAnlEvent : public TGo4EventElement {
   public:
      TXXXAnlEvent() : TGo4EventElement() {}
      TXXXAnlEvent(const char* name) : TGo4EventElement(name) {}
      virtual ~TXXXAnlEvent() {}

      virtual void  Clear(Option_t *t="");

      Float_t frData[XXX_NUM_CHAN];

   ClassDef(TXXXAnlEvent,1)
};
#endif //TXXXANLEVENT_H



