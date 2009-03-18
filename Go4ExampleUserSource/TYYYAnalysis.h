#ifndef TYYYANALYSIS_H
#define TYYYANALYSIS_H

#include "TGo4Analysis.h"

class TH1D;
class TFile;

class TYYYRawEvent;
class TYYYUnpackEvent;
class TYYYParameter;

class TYYYAnalysis : public TGo4Analysis {
   public:
      TYYYAnalysis();
      TYYYAnalysis(const char* lmd, const char* raw, const char* cal);
      virtual ~TYYYAnalysis() ;
      virtual Int_t UserPreLoop();
      virtual Int_t UserEventFunc();
      virtual Int_t UserPostLoop();
   private:
      TFile           *fUserFile;
      TYYYRawEvent    *fRawEvent;
      TYYYUnpackEvent *fUnpackEvent;
      TH1D            *fSize;
      TYYYParameter   *fPar;
      Int_t            fEvents;
      Int_t            fLastEvent;

   ClassDef(TYYYAnalysis,1)
};
#endif //TYYYANALYSIS_H



