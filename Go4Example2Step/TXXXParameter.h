// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TXXXParameter_H
#define TXXXParameter_H

#include "TGo4Parameter.h"

#include "TArrayI.h"

class TXXXParameter : public TGo4Parameter {
   public:
      TXXXParameter(const char *name = nullptr);
      virtual ~TXXXParameter() {}

      Float_t frP1{0.}; // Offset for calibration
      Float_t frP2{0.}; // Factor for Calibration
      Bool_t fbHisto{kFALSE}; // Enable Histogramming

      TArrayI fArr;  // example of usage of TArray in the parameter

      Int_t fArr2[3][4]; // example of 2d array usage

      Int_t fArr3[3][4][5]; // example of 3d array usage (works only with browser)

   ClassDefOverride(TXXXParameter,2)
};

#endif
