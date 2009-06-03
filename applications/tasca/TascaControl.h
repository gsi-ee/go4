//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

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
      void init(){
    	  checkTof=kFALSE;
    	  checkChopper=kFALSE;
    	  checkMacro=kFALSE;
    	  checkMicro=kFALSE;
    	  TofMustbe=kTRUE;
    	  ChopperMustbe=kTRUE;
    	  MacroMustbe=kTRUE;
    	  MicroMustbe=kTRUE;
    	  UnpackHisto=kTRUE;
    	  CaliHisto=kTRUE;
    	  CheckHisto=kTRUE;
    	  AnlHisto=kTRUE;
      }

	Bool_t checkTof;
	Bool_t checkChopper;
	Bool_t checkMacro;
	Bool_t checkMicro;
	Bool_t TofMustbe;//true-false
	Bool_t ChopperMustbe;//true-false
	Bool_t MacroMustbe;//true-false
	Bool_t MicroMustbe;//true-false
	Bool_t UnpackHisto;
	Bool_t CaliHisto;
	Bool_t CheckHisto;
	Bool_t AnlHisto;
	UInt_t TofChecked;
	UInt_t TofTrue;
	UInt_t ChopperChecked;
	UInt_t ChopperTrue;
	UInt_t MacroChecked;
	UInt_t MacroTrue;
	UInt_t MicroChecked;
	UInt_t MicroTrue;

   ClassDef(TascaControl,1)
};

#endif
