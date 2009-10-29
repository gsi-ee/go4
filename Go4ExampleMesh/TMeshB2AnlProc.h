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

#ifndef TMESHB2ANLPROCESSOR_H
#define TMESHB2ANLPROCESSOR_H

#include "TGo4EventProcessor.h"

class TMeshB2InputEvent;
class TMeshB2OutputEvent;

class TMeshB2AnlProc : public TGo4EventProcessor {

   public:
      TMeshB2AnlProc() ;
      TMeshB2AnlProc(const char* name);
      void Analysis(TMeshB2OutputEvent* target);
      virtual ~TMeshB2AnlProc() ;

      TMeshB2InputEvent* fxInput;

   ClassDef(TMeshB2AnlProc,1)
};
#endif //TMeshB2AnlProcESSOR_H



