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

#ifndef TMESHRAWEVENT_H
#define TMESHRAWEVENT_H

#include "TGo4EventElement.h"

#include "TMeshB1InputEvent.h"
#include "TMeshB2InputEvent.h"
#include "TMeshB3InputEvent.h"

class TMeshUnpackProc;
class TGo4FileSource;

class TMeshRawEvent : public TGo4EventElement {
   public:
      TMeshRawEvent();
      TMeshRawEvent(const char *name);
      virtual ~TMeshRawEvent();
      /**
       * Method called by the event owner (analysis step) to fill the
       * event element from the set event source. Event source can
       * be the source of the analysis step (if this is a raw event)
       * or the event processor (if this is a reduced event).
       */
      Int_t Fill() override;

      /**
       * Method called by the event owner (analysis step) to clear the
       * event element.
       */
      void Clear(Option_t *opt = "") override;
      Int_t Init() override;

      /** we compose data structure from subbranches: */
      TMeshB1InputEvent fxSub1;
      TMeshB2InputEvent fxSub2;
      TMeshB3InputEvent fxSub3;

   private:
      TMeshUnpackProc *fxUnpacker{nullptr};  //! Don't put this to file
      TGo4FileSource  *fxFile{nullptr};  //! Don't put this to file

   ClassDefOverride(TMeshRawEvent,1)
};

#endif //TMESHRAWEVENT_H

