#ifndef TascaControl_H
#define TascaControl_H

#include "TGo4Parameter.h"


class TascaControl : public TGo4Parameter {
   public:
      TascaControl();
      TascaControl(const char* name);
      virtual ~TascaControl();

      Int_t PrintParameter();
      Bool_t UpdateFrom(TGo4Parameter *);

      Bool_t UnpackHisto;
      Bool_t CaliHisto;
      Bool_t AnlHisto;

   ClassDef(TascaControl,1)
};

#endif
