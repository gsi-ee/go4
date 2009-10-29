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

#ifndef TMESHPROVIDERPROCESSOR_H
#define TMESHPROVIDERPROCESSOR_H

#include "TGo4EventProcessor.h"

class TMeshProviderProc : public TGo4EventProcessor {

   public:
      TMeshProviderProc();
      TMeshProviderProc(const char* name);
      virtual ~TMeshProviderProc() ;

      /** This method will check if input event is correct and
      * might reset the input event pointer for the subsequent steps */
      void SetRealInput();

   ClassDef(TMeshProviderProc,1)
};
#endif //TMeshProviderProcESSOR_H
