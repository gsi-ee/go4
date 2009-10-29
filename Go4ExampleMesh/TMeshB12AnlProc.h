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

#ifndef TMESHB12ANLPROCESSOR_H
#define TMESHB12ANLPROCESSOR_H

#include "TGo4EventProcessor.h"

class TMeshB12OutputEvent;
class TMeshB1OutputEvent;
class TMeshB2OutputEvent;

class TMeshB12AnlProc : public TGo4EventProcessor {
   public:
      TMeshB12AnlProc() ;
      TMeshB12AnlProc(const char* name);
      virtual ~TMeshB12AnlProc() ;
      void Analysis(TMeshB12OutputEvent* target);

      TMeshB1OutputEvent* fxInput1;
      TMeshB2OutputEvent* fxInput2;

   ClassDef(TMeshB12AnlProc,1)
};
#endif //TMeshB12AnlProcESSOR_H



