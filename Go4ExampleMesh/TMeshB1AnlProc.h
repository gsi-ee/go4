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

#ifndef TMESHB1ANLPROCESSOR_H
#define TMESHB1ANLPROCESSOR_H

#include "TGo4EventProcessor.h"

class TMeshB1InputEvent;
class TMeshB1OutputEvent;

class TMeshB1AnlProc : public TGo4EventProcessor {

   public:
      TMeshB1AnlProc() ;
      TMeshB1AnlProc(const char* name);
      void Analysis(TMeshB1OutputEvent* target);
      virtual ~TMeshB1AnlProc() ;

      TMeshB1InputEvent* fxInput;

   ClassDef(TMeshB1AnlProc,1)
};

#endif //TMeshB1AnlProcESSOR_H
