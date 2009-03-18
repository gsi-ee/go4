#ifndef TUNPACKPROCESSOR_H
#define TUNPACKPROCESSOR_H

#include "TGo4EventProcessor.h"

class TXXXParam;
class TXXXControl;

class TXXXProc : public TGo4EventProcessor {
   public:
      TXXXProc() ;
      TXXXProc(const char* name);
      virtual ~TXXXProc() ;

      Bool_t BuildEvent(TGo4EventElement* target); // event processing function

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
      TGo4PolyCond  *fPolyCon;
      TGo4CondArray *fConArr;
      TGo4MbsEvent  *fInput;
      TXXXParam      *fParam;
      TXXXControl    *fControl;
      TGo4Picture   *fPicture;
      TGo4Picture   *fcondSet;

   ClassDef(TXXXProc,1)
};

#endif //TUNPACKPROCESSOR_H

