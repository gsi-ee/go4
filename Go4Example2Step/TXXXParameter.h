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

#ifndef TXXXParameter_H
#define TXXXParameter_H

#include "TGo4Parameter.h"

#define __ARRAYSIZE__ 100
#define __MEVX__ 5
#define __MEVY__ 10


class TXXXParameter : public TGo4Parameter {
   public:
      TXXXParameter();
      TXXXParameter(const char* name);
      virtual ~TXXXParameter();

      Int_t PrintParameter(Text_t * n, Int_t);
      Bool_t UpdateFrom(TGo4Parameter *);
      Float_t frP1;
      Float_t frP2;
      Int_t fiNum;
      Bool_t fbEnab;
      Double_t fdTest;
      Short_t fsTest;
      Char_t fcTest;
      Long_t flTest;
      TString fxInputfile;
      Int_t fiDataArray[__ARRAYSIZE__];
      Double_t fdMeV[__MEVX__][__MEVY__];
      UInt_t fuNum;
      Double_t fdTestarr[__MEVX__];
      UShort_t fuShort;
      UChar_t fuChar;
      ULong_t fuLong;

   ClassDef(TXXXParameter,1)
};

#endif
