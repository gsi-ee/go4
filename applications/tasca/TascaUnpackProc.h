#ifndef TascaUNPACKPROCESSOR_H
#define TascaUNPACKPROCESSOR_H

#include "TGo4EventProcessor.h"
#include "TascaCodec.h"

class TascaParameter;
class TascaUnpackEvent;

class TascaUnpackProc : public TGo4EventProcessor {
   public:
      TascaUnpackProc();
      TascaUnpackProc(const char* name);
      virtual ~TascaUnpackProc() ;
      void TascaUnpack(TascaUnpackEvent* target);
  private:
		TascaCodec *codec;
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
      TascaParameter *fParam1;
      TascaParameter *fParam2;
      TGo4Picture   *Picture1;
      TGo4Picture   *fcondSet;

   ClassDef(TascaUnpackProc,1)
};

#endif //TascaUNPACKPROCESSOR_H
