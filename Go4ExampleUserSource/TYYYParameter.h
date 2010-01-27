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

#ifndef TYYYParameter_H
#define TYYYParameter_H

#define __ARRAYSIZE__ 5

#include "TGo4Parameter.h"

class TGo4Fitter;

class TYYYParameter : public TGo4Parameter {
   public:
      TYYYParameter();
      TYYYParameter(const char* name);
      virtual ~TYYYParameter();
      virtual Int_t PrintParameter(Text_t * n, Int_t);
      virtual Bool_t UpdateFrom(TGo4Parameter *);

      Float_t frP1;
      Float_t frP2;
      Int_t fiDataArray[__ARRAYSIZE__];
      TGo4Fitter* fxFitArray[__ARRAYSIZE__];

   ClassDef(TYYYParameter,1)
};

#endif //TYYYParameter_H





