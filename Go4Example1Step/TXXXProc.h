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
      TH1           *fCr1Ch[8];
      TH1           *fCr2Ch[8];
      TH2           *fCr1Ch1x2;
      TH1           *fHis1;
      TH1           *fHis1gate;
      TH1           *fHis2;
      TH1           *fHis2gate;
      TGo4WinCond   *fconHis1;
      TGo4WinCond   *fconHis2;
      TGo4PolyCond  *fPolyCon;
      TGo4CondArray *fConArr;
      TXXXParam     *fParam;
      TXXXControl   *fControl;
      TGo4Picture   *fPicture;
      TGo4Picture   *fcondSet;

   ClassDef(TXXXProc,1)
};

#endif //TUNPACKPROCESSOR_H

