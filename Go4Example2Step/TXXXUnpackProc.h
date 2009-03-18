#ifndef TXXXUNPACKPROCESSOR_H
#define TXXXUNPACKPROCESSOR_H

#include "TGo4EventProcessor.h"

class TXXXParameter;
class TXXXUnpackEvent;

class TXXXUnpackProc : public TGo4EventProcessor {
   public:
      TXXXUnpackProc();
      TXXXUnpackProc(const char* name);
      virtual ~TXXXUnpackProc() ;
      void XXXUnpack(TXXXUnpackEvent* target);
  private:
      TH1I          *fCr1Ch[8];
      TH1I          *fCr2Ch[8];
      TH2I          *fCr1Ch1x2;
      TH1I          *fHis1;
      TH1I          *fHis1gate;
      TH1I          *fHis2;
      TH1I          *fHis2gate;
      TGo4WinCond   *fconHis1;
      TGo4WinCond   *fconHis2;
      TGo4WinCond   *fWinCon1;
      TGo4WinCond   *fWinCon2;
      TGo4PolyCond  *fPolyCon1;
      TGo4CondArray *fConArr1;
      TGo4CondArray *fConArr2;
      TGo4MbsEvent  *fInput;
      TXXXParameter *fParam1;
      TXXXParameter *fParam2;
      TGo4Picture   *Picture1;
      TGo4Picture   *fcondSet;

   ClassDef(TXXXUnpackProc,1)
};

#endif //TXXXUNPACKPROCESSOR_H
