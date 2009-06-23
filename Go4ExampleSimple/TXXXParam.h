#ifndef TXXXParam_H
#define TXXXParam_H

#include "TGo4Parameter.h"

class TXXXParam : public TGo4Parameter {
   public:
      TXXXParam(const char* name = 0);
      virtual ~TXXXParam();
      virtual Int_t  PrintParameter(Text_t * n, Int_t);
      virtual Bool_t UpdateFrom(TGo4Parameter *);

      Bool_t   fillRaw;  // control filling of raw histograms

   ClassDef(TXXXParam,1)
};

#endif //SPAR_H
