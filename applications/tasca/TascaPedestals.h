#ifndef TascaPedestals_H
#define TascaPedestals_H

#include "TGo4Parameter.h"


class TascaPedestals : public TGo4Parameter {
   public:
      TascaPedestals();
      TascaPedestals(const char* name);
      virtual ~TascaPedestals();

      Int_t   PrintPedestals();
      Bool_t  UpdateFrom(TGo4Parameter *);
      void    SetPedestals(Int_t i, Float_t value){ffPedestals[i]=value;return;}
      Float_t GetPedestals(Int_t i){return ffPedestals[i];}
      void    SavePedestals(const char * file);
      void    RestorePedestals(const char * file);

      Bool_t fbRestore;
      Bool_t fbSave;
      TString fxFile;
      Float_t ffPedestals[96];

   ClassDef(TascaPedestals,1)
};

#endif
