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

      TH1I             *fSum1;
      TH1I             *fSum2;
      TH1I             *fSum3;
      TH1I             *fCaliSum1;
      TGraph           *fCaligraph;

      TGraph           *fxGraph1;
      TGraph           *fxGraph2;
      TMultiGraph      *fxMultiGraph;

      TascaUnpackEvent  *fInput;
      TascaParameter    *fParam1;
      TascaParameter    *fParam2;
      TascaCalibPar     *fCalipar;
      TGo4WinCond      *fWinCon;

   ClassDef(TascaAnlProc,1)
};
#endif //TascaANLPROCESSOR_H
