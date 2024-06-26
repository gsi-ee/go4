// $Id: TXXXParameter.h 478 2009-10-29 12:26:09Z linev $
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

#define __ARRAYSIZE__ 100
#define __MEVX__ 5
#define __MEVY__ 10

class TXXXParameter : public TGo4Parameter {
   public:
      TXXXParameter(const char *name = nullptr);

      Float_t frP1{0};
      Float_t frP2{0};
      Int_t fiNum{0};
      Bool_t fbEnab{kFALSE};
      Double_t fdTest{0};
      Short_t fsTest{0};
      Char_t fcTest{0};
      Long_t flTest{0};
      TString fxInputfile;
      Int_t fiDataArray[__ARRAYSIZE__];
      Double_t fdMeV[__MEVX__][__MEVY__];
      UInt_t fuNum{0};
      Double_t fdTestarr[__MEVX__];
      UShort_t fuShort{0};
      UChar_t fuChar{0};
      ULong_t fuLong{0};

   ClassDefOverride(TXXXParameter,1)
};

#endif
