#ifndef TUNPACKPROCESSOR_H
#define TUNPACKPROCESSOR_H

#include "TGo4EventProcessor.h"

class TXXXParam;

class TXXXProc : public TGo4EventProcessor {
   public:
      TXXXProc() ;
      TXXXProc(const char* name);
      virtual ~TXXXProc() ;

      Bool_t BuildEvent(TGo4EventElement* target); // event processing function

 private:
      TH1          *fCr1Ch[8];
      TH1          *fCr2Ch[8];
      TH2          *fCr1Ch1x2;
      TH1          *fHis1;
      TH1          *fHis1gate;
      TH1          *fHis2;
      TH1          *fHis2gate;
      TGo4WinCond   *fconHis1;
      TGo4WinCond   *fconHis2;
      TGo4PolyCond  *fPolyCon;
      TGo4CondArray *fConArr;
      TGo4MbsEvent  *fInput;
      TXXXParam      *fParam;
      TGo4Picture   *fPicture;
      TGo4Picture   *fcondSet;
      Float_t        fCrate1[8];
      Float_t        fCrate2[8];

   ClassDef(TXXXProc,1)
};
#endif //TUNPACKPROCESSOR_H

