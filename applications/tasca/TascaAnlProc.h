#ifndef TascaANLPROCESSOR_H
#define TascaANLPROCESSOR_H

#include "TGo4EventProcessor.h"

class TascaCaliEvent;
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
      TascaParameter *fParam;
      TH2D *fStop;

   ClassDef(TascaAnlProc,1)
};
#endif //TascaANLPROCESSOR_H
