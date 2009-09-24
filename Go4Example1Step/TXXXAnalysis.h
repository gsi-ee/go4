#ifndef TANALYSIS_H
#define TANALYSIS_H

#include "TGo4Analysis.h"

class TGo4MbsEvent;
class TXXXControl;

class TXXXAnalysis : public TGo4Analysis {
   public:
      TXXXAnalysis();
      TXXXAnalysis(const char* name);
      virtual ~TXXXAnalysis() ;
      virtual Int_t UserPreLoop();
      virtual Int_t UserEventFunc();
      virtual Int_t UserPostLoop();
   private:
      TGo4MbsEvent *fMbsEvent;
      TXXXControl   *fCtl;
      Int_t         fEvents;
      Int_t         fLastEvent;

   ClassDef(TXXXAnalysis,1)
};
#endif //TANALYSIS_H
