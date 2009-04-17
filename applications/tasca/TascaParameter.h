#ifndef TascaParameter_H
#define TascaParameter_H

#include "TGo4Parameter.h"


class TascaParameter : public TGo4Parameter {
   public:
      TascaParameter();
      TascaParameter(const char* name);
      virtual ~TascaParameter();

      Int_t PrintParameter(Text_t * n, Int_t);
      Bool_t UpdateFrom(TGo4Parameter *);
      void SetPedestal(Int_t channel, Float_t value);
      void SavePedestal();

      Bool_t fbRestore;
      Bool_t fbSave;
      TString fxFile;
      Float_t ffPedestals[96];

   ClassDef(TascaParameter,1)
};

#endif
