//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef TascaParameter_H
#define TascaParameter_H

#include "TGo4Parameter.h"


class TascaParameter : public TGo4Parameter {
   public:
      TascaParameter();
      TascaParameter(const char* name);
      virtual ~TascaParameter();

      Int_t PrintParameter();
      Bool_t IsPrinted(){return Printed;}
      Bool_t UpdateFrom(TGo4Parameter *);

      Bool_t Save;
      Bool_t Fill;
      Int_t  shift;
      UInt_t EventStackSize;
      Bool_t Printed; //! not to store
      UInt_t Adc80TofMin;
      UInt_t AdcThreshold;
      Float_t AlphaMaxL;
      Float_t AlphaMaxH;
      Float_t AlphaMinL;
      Float_t AlphaMinH;
      UInt_t AlphaTmin;
      UInt_t AlphaTmax;
      UInt_t Alpha1Tmin;
      UInt_t Alpha1Tmax;
      UInt_t Alpha2Tmin;
      UInt_t Alpha2Tmax;
      UInt_t Fission1Tmin;
      UInt_t Fission1Tmax;
      UInt_t Fission2Tmin;
      UInt_t Fission2Tmax;
      UInt_t SystemTimeSecOff;

   ClassDef(TascaParameter,1)
};

#endif
