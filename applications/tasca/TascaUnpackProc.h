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
	  void savePedestals();
		TascaCodec *codec;
      TH1I          *fM1Ch[32];
      TH1I          *fM2Ch[32];
      TH1I          *fM3Ch[32];
      TH1I  		*fPed1;
      TH1I  		*fPed2;
      TH1I  		*fPed3;
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
      TGo4Picture   *M1raw;
      TGo4Picture   *M2raw;
      TGo4Picture   *M3raw;
      TGo4Picture   *fcondSet;
      Int_t latches[5];
      Int_t patterns[5];
      Int_t i,k, evcount;

   ClassDef(TascaUnpackProc,1)
};

#endif //TascaUNPACKPROCESSOR_H
