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

#ifndef TXXXControl_H
#define TXXXControl_H

#include "TGo4Parameter.h"

class TXXXControl : public TGo4Parameter {
   public:
      TXXXControl(const char* name = 0);

      Bool_t   fill; // control filling of histograms

   ClassDef(TXXXControl,1)
};

#endif // TXXXControl_H
