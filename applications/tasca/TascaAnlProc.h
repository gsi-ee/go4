//---------------------------------------------
// Go4 Tasca analysis 
// Author: Hans G. Essel 
//         H.Essel@gsi.de 
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef TascaANLPROCESSOR_H
#define TascaANLPROCESSOR_H

#include "TGo4EventProcessor.h"

class TascaCaliEvent;
class TascaControl;
class TascaParameter;
class TascaAnlEvent;
class TascaAnalysis;

class TascaAnlProc : public TGo4EventProcessor {
   public:
      TascaAnlProc() ;
      TascaAnlProc(const char * name);
      virtual ~TascaAnlProc();

      void TascaEventAnalysis(TascaAnlEvent* target);


	  TascaAnalysis  * anl;
      TascaCaliEvent *fInput;
      TascaControl  *fControl;
      TascaParameter *fParam;
      TH2D *fStopLE[48];
      TH2D *fStopHE[48];
      UInt_t i,ii;

   ClassDef(TascaAnlProc,1)
};
#endif //TascaANLPROCESSOR_H
