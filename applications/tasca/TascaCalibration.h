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
      void ReadCoefficients(const char * prefix);
      void ReadSingleCoefficients(const char * file, UInt_t size, Double_t *a0, Double_t *a1, Double_t *a2);
      Float_t CalibrateGammaE(UInt_t channel,UInt_t index){
    	  Double_t c=(Double_t)channel;
    	  return (Float_t) (fdGammaE_a0[index]+fdGammaE_a1[index]*c+fdGammaE_a2[index]*c*c);
      }
      Float_t CalibrateGammaT(UInt_t channel,UInt_t index){
    	  Double_t c=(Double_t)channel;
    	  return (Float_t) (fdGammaT_a0[index]+fdGammaT_a1[index]*c+fdGammaT_a2[index]*c*c);
      }
      Float_t CalibrateStopXL(UInt_t channel,UInt_t index){
    	  Double_t c=(Double_t)channel;
    	  return (Float_t) (fdStopXL_a0[index]+fdStopXL_a1[index]*c+fdStopXL_a2[index]*c*c);
      }
      Float_t CalibrateStopXH(UInt_t channel,UInt_t index){
    	  Double_t c=(Double_t)channel;
    	  return (Float_t) (fdStopXH_a0[index]+fdStopXH_a1[index]*c+fdStopXH_a2[index]*c*c);
      }
      Float_t CalibrateStopYL(UInt_t channel,UInt_t index){
    	  Double_t c=(Double_t)channel;
    	  return (Float_t) (fdStopYL_a0[index]+fdStopYL_a1[index]*c+fdStopYL_a2[index]*c*c);
      }
      Float_t CalibrateStopYH(UInt_t channel,UInt_t index){
    	  Double_t c=(Double_t)channel;
    	  return (Float_t) (fdStopYH_a0[index]+fdStopYH_a1[index]*c+fdStopYH_a2[index]*c*c);
      }
      Float_t CalibrateBackL(UInt_t channel,UInt_t index){
    	  Double_t c=(Double_t)channel;
    	  return (Float_t) (fdBackL_a0[index]+fdBackL_a1[index]*c+fdBackL_a2[index]*c*c);
      }
      Float_t CalibrateBackH(UInt_t channel,UInt_t index){
    	  Double_t c=(Double_t)channel;
    	  return (Float_t) (fdBackH_a0[index]+fdBackH_a1[index]*c+fdBackH_a2[index]*c*c);
      }
      Float_t CalibrateVetoL(UInt_t channel,UInt_t index){
    	  Double_t c=(Double_t)channel;
    	  return (Float_t) (fdVetoL_a0[index]+fdVetoL_a1[index]*c+fdVetoL_a2[index]*c*c);
      }
      Float_t CalibrateVetoH(UInt_t channel,UInt_t index){
    	  Double_t c=(Double_t)channel;
    	  return (Float_t) (fdVetoH_a0[index]+fdVetoH_a1[index]*c+fdVetoH_a2[index]*c*c);
      }
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
