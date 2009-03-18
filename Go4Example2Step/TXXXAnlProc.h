#ifndef TXXXANLPROCESSOR_H
#define TXXXANLPROCESSOR_H

#include "TGo4EventProcessor.h"

class TXXXUnpackEvent;
class TXXXParameter;
class TXXXCalibPar;
class TXXXAnlEvent;

class TXXXAnlProc : public TGo4EventProcessor {
   public:
      TXXXAnlProc() ;
      TXXXAnlProc(const char * name);
      virtual ~TXXXAnlProc();

      void XXXEventAnalysis(TXXXAnlEvent* target);

      TH1I             *fSum1;
      TH1I             *fSum2;
      TH1I             *fSum3;
      TH1I             *fCaliSum1;
      TGraph           *fCaligraph;

      TGraph           *fxGraph1;
      TGraph           *fxGraph2;
      TMultiGraph      *fxMultiGraph;

      TXXXUnpackEvent  *fInput;
      TXXXParameter    *fParam1;
      TXXXParameter    *fParam2;
      TXXXCalibPar     *fCalipar;
      TGo4WinCond      *fWinCon;

   ClassDef(TXXXAnlProc,1)
};
#endif //TXXXANLPROCESSOR_H
