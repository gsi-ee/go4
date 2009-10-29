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

#ifndef TMESHFINALPROCESSOR_H
#define TMESHFINALPROCESSOR_H

#include "TGo4EventProcessor.h"

class TMeshB12OutputEvent;
class TMeshB1OutputEvent;
class TMeshB2OutputEvent;
class TMeshB3OutputEvent;
class TMeshFinalEvent;

class TMeshFinalProc : public TGo4EventProcessor {

   public:
      TMeshFinalProc() ;
      TMeshFinalProc(const char* name);
      void Analysis(TMeshFinalEvent* target);
      virtual ~TMeshFinalProc() ;

      TMeshB1OutputEvent* fxInput1;
      TMeshB2OutputEvent* fxInput2;
      TMeshB3OutputEvent* fxInput3;
      TMeshB12OutputEvent* fxInput12;

   ClassDef(TMeshFinalProc,1)
};

#endif //TMeshFinalProcESSOR_H
