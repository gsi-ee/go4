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

#ifndef TMESHB3INPUTEVENT_H
#define TMESHB3INPUTEVENT_H

#include "TGo4EventElement.h"

class TMeshUnpackProc;
class TGo4FileSource;

class TMeshB3InputEvent : public TGo4EventElement {
   public:
      TMeshB3InputEvent();
      TMeshB3InputEvent(const char* name);
      virtual ~TMeshB3InputEvent();
      Int_t Fill() override;
      void Clear(Option_t *t="") override;
      Int_t Init() override;

      Int_t fiCrate3[16];
   private:
      TMeshUnpackProc *fxProcessor{nullptr};  //! Don't put this to file
      TGo4FileSource  *fxFile{nullptr};  //! Don't put this to file

   ClassDefOverride(TMeshB3InputEvent,1)
};

#endif //TMESHB3INPUTEVENT_H
