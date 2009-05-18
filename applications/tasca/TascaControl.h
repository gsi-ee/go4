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

		Bool_t checkTof;
		Bool_t TofMustbe;//true-false
		Bool_t checkChopper;
		Bool_t ChopperMustbe;//true-false
		Bool_t checkMacro;
		Bool_t MacroMustbe;//true-false
		Bool_t checkMicro;
		Bool_t MicroMustbe;//true-false
      Bool_t UnpackHisto;
      Bool_t CaliHisto;
      Bool_t AnlHisto;
      UInt_t TofChecked;
      UInt_t TofTrue;
      UInt_t ChopperChecked;
      UInt_t ChopperTrue;
      UInt_t MicroChecked;
      UInt_t MicroTrue;
      UInt_t MacroChecked;
      UInt_t MacroTrue;

   ClassDef(TascaControl,1)
};

#endif
