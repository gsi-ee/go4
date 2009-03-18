#ifndef SPAR_H
#define SPAR_H

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

#endif //SPAR_H





