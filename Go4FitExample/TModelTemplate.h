#ifndef TMODELTEMPLATE_H
#define TMODELTEMPLATE_H
#include "../Go4Fit/TGo4FitModel.h"

class TModelTemplate : public TGo4FitModel {
   public:
      TModelTemplate(const char* iName, Int_t iNPars, Bool_t AddAmplitude = kFALSE);

   protected:
      virtual Double_t UserFunction(Double_t* Coordinates, Double_t* Parameters);

   ClassDef(TModelTemplate,1)
};
#endif // TMODELTEMPLATE_H
