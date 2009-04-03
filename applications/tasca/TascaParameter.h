#ifndef TascaParameter_H
#define TascaParameter_H

#include "TGo4Parameter.h"

#define __ARRAYSIZE__ 100
#define __MEVX__ 5
#define __MEVY__ 10


class TascaParameter : public TGo4Parameter {
   public:
      TascaParameter();
      TascaParameter(const char* name);
      virtual ~TascaParameter();

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

   ClassDef(TascaParameter,1)
};

#endif
