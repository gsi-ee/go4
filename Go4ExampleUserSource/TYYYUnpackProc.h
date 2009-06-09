#ifndef TYYYUNPACKPROCESSOR_H
#define TYYYUNPACKPROCESSOR_H

#include "TGo4EventProcessor.h"

class TYYYUnpackEvent;
class TYYYParameter;

class TYYYUnpackProc : public TGo4EventProcessor {
   public:
      TYYYUnpackProc() ;
      TYYYUnpackProc(const char* name);
      void YYYUnpack(TYYYUnpackEvent* target);
      virtual ~TYYYUnpackProc() ;
   private:
      TH1           *fX;
      TH1           *fY;
      TH1           *fZ;
      TH1           *fVX;
      TH1           *fVY;
      TH1           *fVZ;
      TH1           *fNumScatt;
      TH2           *fXY;
      TH2           *fVXVY;
      TH2           *fXYCond;
      TH2           *fVXVYCond;
      TH2           *fEmitX;
      TH2           *fEmitY;
      TH1           *fEmitDist;
      TGo4WinCond   *fWinConR;
      TGo4WinCond   *fWinConV;
      TGo4PolyCond  *fPolyConEmit;
      TYYYParameter *fParam1;

   ClassDef(TYYYUnpackProc,1)
};

#endif //TYYYUNPACKPROCESSOR_H
