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
      TascaEventStack *fEventStack;
  	  TListIter *fStackIter;
  	  TListIter *fEventIter;
  	  TascaEvent *fEvent;
      UInt_t i,ii;
      Bool_t stackfilled;

   ClassDef(TascaAnlProc,1)
};
#endif //TascaAnlPROCESSOR_H
