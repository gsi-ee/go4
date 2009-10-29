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

#ifndef TMESHFINALEVENT_H
#define TMESHFINALEVENT_H

#include "TGo4EventElement.h"

#include "TMeshB1OutputEvent.h"
#include "TMeshB2OutputEvent.h"
#include "TMeshB3OutputEvent.h"
#include "TMeshB12OutputEvent.h"

class TGo4FileSource;
class TMeshFinalProc;

class TMeshFinalEvent : public TGo4EventElement {
   public:
      TMeshFinalEvent() ;
      TMeshFinalEvent(const char* name) ;
      ~TMeshFinalEvent() ;
      virtual Int_t Init();
      virtual Int_t Fill();
      virtual void  Clear(Option_t *t="");

      /** in this example, we include just copies of our sources here:*/
      TMeshB1OutputEvent fxB1;
      TMeshB2OutputEvent fxB2;
      TMeshB3OutputEvent fxB3;
      TMeshB12OutputEvent fxB12;

   private:
      // this object is streamed. Therefore pointers must be excluded!
      // Otherwise these objects are streamed as well
      TMeshFinalProc     * fxFinalProc;  //! Don't put this to file
      TGo4FileSource  * fxFile;  //! Don't put this to file

   ClassDef(TMeshFinalEvent,1)
};

#endif //TXXXANLEVENT_H
