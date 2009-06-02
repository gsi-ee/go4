//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef TascaCheckPROCESSOR_H
#define TascaCheckPROCESSOR_H

#include "TGo4EventProcessor.h"

class TascaCaliEvent;
class TascaControl;
class TascaParameter;
class TascaCheckEvent;
class TascaAnalysis;

class TascaCheckProc : public TGo4EventProcessor {
   public:
      TascaCheckProc() ;
      TascaCheckProc(const char * name);
      virtual ~TascaCheckProc();

      void TascaEventCheck(TascaCheckEvent* target);


	  TascaAnalysis  * anl;
      TascaCaliEvent *fInput;
      TascaControl  *fControl;
      TascaParameter *fParam;
      TGo4WinCond   *fadcKevL;
      TGo4WinCond   *fadcKevH;
      TGo4WinCond   *fgammaKev;
      TH2D *fStopLE[48];
      TH2D *fStopHE[48];
      TH2D *fStopXY;
      TH2D* fAlphaBackL;
      TH2D* fAlphaBackH;
      TH2D* fAlphaGammaL;
      TH2D* fAlphaGammaH;
      TH2D* fAlphaVetoL;
      TH2D* fAlphaVetoH;
      UInt_t i,ii;

   ClassDef(TascaCheckProc,1)
};
#endif //TascaCheckPROCESSOR_H
