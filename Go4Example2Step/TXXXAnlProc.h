#ifndef TXXXANLPROCESSOR_H
#define TXXXANLPROCESSOR_H

#include "TGo4EventProcessor.h"

class TXXXParameter;
class TXXXCalibPar;
class TXXXAnlEvent;
class TGo4Fitter;

class TXXXAnlProc : public TGo4EventProcessor {
   public:
      TXXXAnlProc() ;
      TXXXAnlProc(const char * name);
      virtual ~TXXXAnlProc();

      void XXXEventAnalysis(TXXXAnlEvent* target);

      TH1              *fSum1;
      TH1              *fSum2;
      TH1              *fSum3;
      TH1              *fCaliSum1;
      TGraph           *fCaligraph;

      TXXXParameter    *fParam1;
      TXXXParameter    *fParam2;
      TXXXCalibPar     *fCalipar;
      TGo4WinCond      *fWinCon;

      TGo4Fitter       *fFitter;
      Int_t             fFitCounter;
      TH1              *fFitSrc;
      TH1              *fFitRes;

   ClassDef(TXXXAnlProc,1)
};
#endif //TXXXANLPROCESSOR_H
