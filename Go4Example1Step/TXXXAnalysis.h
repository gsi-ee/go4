#ifndef TANALYSIS_H
#define TANALYSIS_H

#include "TGo4Analysis.h"

class TFile;
class TGo4MbsEvent;
class TXXXParam;
class TXXXControl;

class TXXXAnalysis : public TGo4Analysis {
   public:
      TXXXAnalysis();
      TXXXAnalysis(const char* input, Int_t type, Int_t port, const char* out, Bool_t enable);
      virtual ~TXXXAnalysis() ;
      virtual Int_t UserPreLoop();
      virtual Int_t UserEventFunc();
      virtual Int_t UserPostLoop();
   private:
      TFile        *fUserFile;
      TGo4MbsEvent *fMbsEvent;
      TXXXParam     *fPar;
      TXXXControl   *fCtl;
      Int_t         fEvents;
      Int_t         fLastEvent;

   ClassDef(TXXXAnalysis,1)
};
#endif //TANALYSIS_H
