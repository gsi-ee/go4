#ifndef TMESHANALYSIS_H
#define TMESHANALYSIS_H

#include "TGo4Analysis.h"

class TGo4MbsEvent;
class TMeshParameter;
class TH1D;

class TMeshAnalysis : public TGo4Analysis {
   public:
      TMeshAnalysis();
      TMeshAnalysis(int argc, char** argv);
      virtual ~TMeshAnalysis() ;
      virtual Int_t UserPreLoop();
      virtual Int_t UserEventFunc();
      virtual Int_t UserPostLoop();
   private:

      TMeshParameter     *fPar;
      TGo4MbsEvent       *fMbsEvent;//!
      TH1D               *fSize;
      Int_t               fEvents;
      Int_t               fLastEvent;

      ClassDef(TMeshAnalysis,1)
};
#endif //TMESHANALYSIS_H



