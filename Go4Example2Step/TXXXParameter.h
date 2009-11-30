// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TXXXParameter_H
#define TXXXParameter_H

#include "TGo4Parameter.h"

class TXXXParameter : public TGo4Parameter {
   public:
      TXXXParameter();
      TXXXParameter(const char* name);
      virtual ~TXXXParameter();

      Int_t PrintParameter(Text_t * n=0, Int_t t=0);
      Bool_t UpdateFrom(TGo4Parameter *);

      Float_t frP1; // Offset for calibration
      Float_t frP2; // Factor for Calibration
      Bool_t fbHisto; // Enable Histogramming

   ClassDef(TXXXParameter,1)
};

#endif
