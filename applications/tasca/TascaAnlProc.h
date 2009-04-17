#ifndef TascaANLPROCESSOR_H
#define TascaANLPROCESSOR_H

#include "TGo4EventProcessor.h"

class TascaUnpackEvent;
class TascaParameter;
class TascaCalibPar;
class TascaAnlEvent;

class TascaAnlProc : public TGo4EventProcessor {
   public:
      TascaAnlProc() ;
      TascaAnlProc(const char * name);
      virtual ~TascaAnlProc();

      void TascaEventAnalysis(TascaAnlEvent* target);


      TascaUnpackEvent  *fInput;
      TascaParameter    *fParPed;
      TH2D *fStop;

   ClassDef(TascaAnlProc,1)
};
#endif //TascaANLPROCESSOR_H
