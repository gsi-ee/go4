#ifndef TascaParameter_H
#define TascaParameter_H

#include "TGo4Parameter.h"


class TascaParameter : public TGo4Parameter {
   public:
      TascaParameter();
      TascaParameter(const char* name);
      virtual ~TascaParameter();

      Int_t PrintParameter();
      Bool_t UpdateFrom(TGo4Parameter *);

      Bool_t Save;
      Bool_t Fill;
      Int_t  shift;

   ClassDef(TascaParameter,1)
};

#endif
