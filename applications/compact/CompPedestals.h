//---------------------------------------------
// Go4 Comp analysis 
// Author: Hans G. Essel 
//         H.Essel@gsi.de 
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef CompPedestals_H
#define CompPedestals_H

#include "TGo4Parameter.h"


class CompPedestals : public TGo4Parameter {
   public:
      CompPedestals();
      CompPedestals(const char* name);
      virtual ~CompPedestals();

      Int_t   PrintPedestals();
      Bool_t  UpdateFrom(TGo4Parameter *);
      void    SetPedestals(Int_t i, Float_t value){ffPedestals[i]=value;return;}
      Float_t GetPedestals(Int_t i){return ffPedestals[i];}
      void    SavePedestals(const char * file);
      void    RestorePedestals(const char * file);

      Bool_t Restore;
      Bool_t Save;
      Bool_t Calibrate;
      TString File;
      Float_t ffOffset;
      Float_t ffPedestals[96];

   ClassDef(CompPedestals,1)
};

#endif
