#ifndef TascaCalibration_H
#define TascaCalibration_H

#include "TGo4Parameter.h"


class TascaCalibration : public TGo4Parameter {
   public:
      TascaCalibration();
      TascaCalibration(const char* name);
      virtual ~TascaCalibration();

      Int_t PrintParameter();
      Bool_t UpdateFrom(TGo4Parameter *);

      Bool_t fbSave;
      Float_t ffGammaEa0[8];
      Float_t ffGammaEa1[8];
      Float_t ffGammaEa2[8];
      Float_t ffGammaTa0[8];
      Float_t ffGammaTa1[8];
      Float_t ffGammaTa2[8];

   ClassDef(TascaCalibration,1)
};

#endif
