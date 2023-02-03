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

#ifndef TMESHDUMMYEVENT_H
#define TMESHDUMMYEVENT_H

#include "TGo4EventElement.h"

class TMeshProviderProc;

/** The only purpose for this class is to trigger the method of
* the corresponding provider processor to get the correct event
* pointer from framework. */

class TMeshDummyEvent : public TGo4EventElement {
   public:
      TMeshDummyEvent();
      TMeshDummyEvent(const char *name);
      virtual ~TMeshDummyEvent();

      Int_t Fill() override;
      void  Clear(Option_t *opt = "") override;
      Int_t Init() override;

   private:
      TMeshProviderProc  *fxProviderProc{nullptr};//!

   ClassDefOverride(TMeshDummyEvent,1)
};

#endif //TMESHDUMMYANLEVENT_H
