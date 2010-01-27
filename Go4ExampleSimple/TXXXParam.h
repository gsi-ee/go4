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

#ifndef TXXXParam_H
#define TXXXParam_H

#include "TGo4Parameter.h"

class TXXXParam : public TGo4Parameter {
   public:
      TXXXParam(const char* name = 0);

      Bool_t   fillRaw;  // control filling of raw histograms

   ClassDef(TXXXParam,1)
};

#endif // TXXXParam_H
