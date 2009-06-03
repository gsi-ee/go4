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
      Bool_t UpdateFrom(TGo4Parameter *);

      Bool_t Save;
      Bool_t Fill;
      Int_t  shift;
      UInt_t EventStackSize;

   ClassDef(TascaParameter,1)
};

#endif
