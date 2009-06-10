//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef TascaAnlPROCESSOR_H
#define TascaAnlPROCESSOR_H

#include "TGo4EventProcessor.h"

class TascaCheckEvent;
class TascaControl;
class TascaParameter;
class TascaAnlEvent;
class TascaAnalysis;
class TascaEvent;
class TascaEventStack;
#include "TList.h"

class TascaAnlProc : public TGo4EventProcessor {
   public:
      TascaAnlProc() ;
      TascaAnlProc(const char * name);
      virtual ~TascaAnlProc();

      void TascaEventAnalysis(TascaAnlEvent* target);


	  TascaAnalysis  * anl;
      TascaCheckEvent *fInput;
      TascaControl  *fControl;
      TascaParameter *fParam;
      TGo4WinCond   *fwinEVR;
      TGo4WinCond   *fwinAlpha;
      TGo4WinCond   *fwinFission1;
      TGo4WinCond   *fwinAlpha1;
      TGo4WinCond   *fwinAlpha2;
      TGo4WinCond   *fwinFission2;
      TGo4WinCond   *fwinBack;
      TascaEventStack *fEventStack;
  	  TListIter *fStackIter;
  	  TListIter *fEventIter;
  	  TascaEvent *fEvent;
  	  TascaEvent *fFissionEvent;
  	  TascaEvent *fAlphaEvent;
  	  TascaEvent *fStackEvent;
      UInt_t i,ii;
      UInt_t fFissions;
      UInt_t fAlphas;
      UInt_t fEvrs;
      UInt_t fFirstEvent;
      Bool_t fStackfilled;
      Bool_t fAlphaFound;
      UInt_t fTimeDiff;

   ClassDef(TascaAnlProc,1)
};
#endif //TascaAnlPROCESSOR_H
