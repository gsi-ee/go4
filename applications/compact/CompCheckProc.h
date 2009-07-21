//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef CompCheckPROCESSOR_H
#define CompCheckPROCESSOR_H

#include "TGo4EventProcessor.h"

class CompCaliEvent;
class CompControl;
class CompParameter;
class CompCheckEvent;
class CompAnalysis;

class CompCheckProc : public TGo4EventProcessor {
   public:
      CompCheckProc() ;
      CompCheckProc(const char * name);
      virtual ~CompCheckProc();

      void CompEventCheck(CompCheckEvent* target);


	  CompAnalysis  * anl;
      CompCaliEvent *fInput;
      CompControl  *fControl;
      CompParameter *fParam;
      TGo4WinCond   *fadcKevL;
      TGo4WinCond   *fadcKevH;
      TGo4WinCond   *fgammaKev;
      TGo4WinCond   *fgammaMysec;
      TGo4WinCond   *fwinEvr;
      TGo4WinCond   *fwinAlpha;
      TGo4WinCond   *fwinFission1;
      TGo4WinCond   *fwinAlpha1;
      TGo4WinCond   *fwinAlpha2;
      TGo4WinCond   *fwinFission2;
      TGo4WinCond   *fwinBack;
      TH2D *fStopLE[48];
      TH2D *fStopHE[48];
      TH2D *fStopXY;
      TH2D* fAlphaBackL;
      TH2D* fAlphaBackH;
      TH2D* fAlphaGammaL;
      TH2D* fAlphaGammaH;
      TH2D* fAlphaVetoL;
      TH2D* fAlphaVetoH;
      TH1I	*fTime;
      UInt_t TimeLastgamma;
      UInt_t TimeLastmysec;
      UInt_t TimeLastadc;
      UInt_t i,ii;
      UInt_t fiEventsProcessed;
      UInt_t fiEventsWritten;
      Int_t fLastEvent;

   ClassDef(CompCheckProc,1)
};
#endif //CompCheckPROCESSOR_H
