#ifndef TascaCalibration_H
#define TascaCalibration_H

#include "TGo4Parameter.h"
#include "TMath.h"

class TascaCalibration : public TGo4Parameter {
   public:
      TascaCalibration();
      TascaCalibration(const char* name);
      virtual ~TascaCalibration();

      Int_t PrintParameter();
      Bool_t Preset();
      Bool_t UpdateFrom(TGo4Parameter *);
      void ReadCoefficients(const char * file);

      /** Calibration polynom coeff */
      Double_t fdGammaE_a0[8];
      Double_t fdGammaE_a1[8];
      Double_t fdGammaE_a2[8];
      Double_t fdGammaT_a0[8];
      Double_t fdGammaT_a1[8];
      Double_t fdGammaT_a2[8];

      Double_t fdStopXL_a0[144];
      Double_t fdStopXL_a1[144];
      Double_t fdStopXL_a2[144];
      Double_t fdStopXH_a0[144];
      Double_t fdStopXH_a1[144];
      Double_t fdStopXH_a2[144];

      Double_t fdStopYL_a0[96];
      Double_t fdStopYL_a1[96];
      Double_t fdStopYL_a2[96];
      Double_t fdStopYH_a0[96];
      Double_t fdStopYH_a1[96];
      Double_t fdStopYH_a2[96];

      Double_t fdBackL_a0[64];
      Double_t fdBackL_a1[64];
      Double_t fdBackL_a2[64];
      Double_t fdBackH_a0[64];
      Double_t fdBackH_a1[64];
      Double_t fdBackH_a2[64];

      Double_t fdVetoL_a0[16];
      Double_t fdVetoL_a1[16];
      Double_t fdVetoL_a2[16];
      Double_t fdVetoH_a0[16];
      Double_t fdVetoH_a1[16];
      Double_t fdVetoH_a2[16];

   ClassDef(TascaCalibration,1)
};

#endif
