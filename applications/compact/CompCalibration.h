//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef CompCalibration_H
#define CompCalibration_H

#include "TGo4Parameter.h"
#include "TMath.h"

class CompCalibration : public TGo4Parameter {
   public:
      CompCalibration();
      CompCalibration(const char* name);
      virtual ~CompCalibration();

      Int_t PrintParameter();
      Bool_t Preset();
      Bool_t UpdateFrom(TGo4Parameter *);
      void ReadCoefficients();
      void ReadSingleCoefficients(const char * file, UInt_t size, Double_t *a0, Double_t *a1, Double_t *a2);
      inline Float_t CalibrateGammaE(UInt_t channel,UInt_t index){
    	  Double_t c=(Double_t)channel;
    	  return (Float_t) (fdGammaE_a0[index]+(fdGammaE_a1[index]+fdGammaE_a2[index]*c)*c);
      }
      inline UInt_t CalibrateGammaT(UInt_t channel){
    	  Double_t c=(Double_t)channel;
    	  return (UInt_t) (fdGammaT_a0+fdGammaT_a1*c);
      }
      inline Float_t CalibrateStopXL(UInt_t channel,Int_t index){
    	  Double_t c=(Double_t)channel;
	  if(index < 0) index=0;
    	  return (Float_t) (fdStopXL_a0[index]+(fdStopXL_a1[index]+fdStopXL_a2[index]*c)*c);
      }
      inline Float_t CalibrateStopXH(UInt_t channel,Int_t index){
    	  Double_t c=(Double_t)channel;
	  if(index < 0) index=0;
    	  return (Float_t) (fdStopXH_a0[index]+(fdStopXH_a1[index]+fdStopXH_a2[index]*c)*c);
      }
      inline Float_t CalibrateStopYL(UInt_t channel,Int_t index){
    	  Double_t c=(Double_t)channel;
	  if(index < 0) index=0;
    	  return (Float_t) (fdStopYL_a0[index]+(fdStopYL_a1[index]+fdStopYL_a2[index]*c)*c);
      }
      inline Float_t CalibrateStopYH(UInt_t channel,Int_t index){
    	  Double_t c=(Double_t)channel;
	  if(index < 0) index=0;
    	  return (Float_t) (fdStopYH_a0[index]+(fdStopYH_a1[index]+fdStopYH_a2[index]*c)*c);
      }
      inline Float_t CalibrateBackL(UInt_t channel,Int_t index){
    	  Double_t c=(Double_t)channel;
	  if(index < 0) index=0;
    	  return (Float_t) (fdBackL_a0[index]+(fdBackL_a1[index]+fdBackL_a2[index]*c)*c);
      }
      inline Float_t CalibrateBackH(UInt_t channel,Int_t index){
    	  Double_t c=(Double_t)channel;
	  if(index < 0) index=0;
    	  return (Float_t) (fdBackH_a0[index]+(fdBackH_a1[index]+fdBackH_a2[index]*c)*c);
      }
      inline Float_t CalibrateVetoL(UInt_t channel,Int_t index){
    	  Double_t c=(Double_t)channel;
	  if(index < 0) index=0;
    	  return (Float_t) (fdVetoL_a0[index]+(fdVetoL_a1[index]+fdVetoL_a2[index]*c)*c);
      }
      inline Float_t CalibrateVetoH(UInt_t channel,Int_t index){
    	  Double_t c=(Double_t)channel;
	  if(index < 0) index=0;
    	  return (Float_t) (fdVetoH_a0[index]+(fdVetoH_a1[index]+fdVetoH_a2[index]*c)*c);
      }
      void EnableCalibration(Bool_t on){
    	  UseCalibration=on;
      }
      void SetPrefix(const char *pref){
    	  prefix=pref;
      }
      Bool_t UseCalibration;
      TString prefix;
      /** Calibration polynom coeff */
      Double_t fdGammaE_a0[8];
      Double_t fdGammaE_a1[8];
      Double_t fdGammaE_a2[8];
      Double_t fdGammaT_a0;
      Double_t fdGammaT_a1;
      Double_t fdGammaT_a2;

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

   ClassDef(CompCalibration,1)
};

#endif
