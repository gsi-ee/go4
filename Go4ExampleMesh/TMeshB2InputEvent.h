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

#ifndef TMESHB2INPUTEVENT_H
#define TMESHB2INPUTEVENT_H

#include "TGo4EventElement.h"

class TMeshUnpackProc;
class TGo4FileSource;

class TMeshB2InputEvent : public TGo4EventElement {
   public:
      TMeshB2InputEvent() ;
      TMeshB2InputEvent(const char* name);
      virtual ~TMeshB2InputEvent() ;

      Int_t Fill();

      void Clear(Option_t *t="");
      Int_t Init();

      Int_t fiCrate2[16];
   private:
      TMeshUnpackProc * fxProcessor;  //! Don't put this to file
      TGo4FileSource * fxFile;  //! Don't put this to file

   ClassDef(TMeshB2InputEvent,1)
};

#endif //TMESHB2INPUTEVENT_H
