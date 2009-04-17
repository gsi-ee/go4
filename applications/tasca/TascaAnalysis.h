#ifndef TascaANALYSIS_H
#define TascaANALYSIS_H

#include "TGo4Analysis.h"

class TFile;
class TH1D;
class TascaParameter;

class TascaAnalysis : public TGo4Analysis  {
   public:
                     TascaAnalysis();
      virtual       ~TascaAnalysis() ;
      virtual Int_t UserPreLoop();
      virtual Int_t UserEventFunc();
      virtual Int_t UserPostLoop();
   private:
      TascaParameter      *fPar;

   ClassDef(TascaAnalysis,1)
};

#endif //TascaANALYSIS_H



