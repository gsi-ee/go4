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

#ifndef TMESHB3ANLPROCESSOR_H
#define TMESHB3ANLPROCESSOR_H

#include "TGo4EventProcessor.h"

class TMeshB3InputEvent;
class TMeshB3OutputEvent;

class TMeshB3AnlProc : public TGo4EventProcessor {

   public:
      TMeshB3AnlProc() ;
      TMeshB3AnlProc(const char* name);
      void Analysis(TMeshB3OutputEvent* target);
      virtual ~TMeshB3AnlProc() ;

      TMeshB3InputEvent* fxInput;

   ClassDef(TMeshB3AnlProc,1)
};

#endif //TMeshB3AnlProcESSOR_H
