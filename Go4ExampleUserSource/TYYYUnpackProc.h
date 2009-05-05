#ifndef TYYYUNPACKPROCESSOR_H
#define TYYYUNPACKPROCESSOR_H

#include "TGo4EventProcessor.h"

class TYYYUnpackEvent;
class TYYYRawEvent;
class TYYYParameter;

class TYYYUnpackProc : public TGo4EventProcessor {
   public:
      TYYYUnpackProc() ;
      TYYYUnpackProc(const char* name);
      void YYYUnpack(TYYYUnpackEvent* target);
      virtual ~TYYYUnpackProc() ;
   private:
      TH1D          *fX;
      TH1D          *fY;
      TH1D          *fZ;
      TH1D          *fVX;
      TH1D          *fVY;
      TH1D          *fVZ;
      TH1D          *fNumScatt;
      TH2D          *fXY;
      TH2D          *fVXVY;
      TH2D          *fXYCond;
      TH2D          *fVXVYCond;
      TH2D          *fEmitX;
      TH2D          *fEmitY;
      TH1D          *fEmitDist;
      TGo4WinCond   *fWinConR;
      TGo4WinCond   *fWinConV;
      TGo4PolyCond  *fPolyConEmit;
      TYYYParameter *fParam1;

   ClassDef(TYYYUnpackProc,1)
};

#endif //TYYYUNPACKPROCESSOR_H
