
#ifndef THadaqANALYSIS_H
#define THadaqANALYSIS_H

#include "TGo4Analysis.h"

class TH1D;
class TGo4MbsEvent;
class THadaqUnpackEvent;
class THadaqParameter;

class THadaqAnalysis : public TGo4Analysis {
   public:
      THadaqAnalysis();
      THadaqAnalysis(int argc, char** argv);
      virtual ~THadaqAnalysis() ;
      virtual Int_t UserPreLoop();
      virtual Int_t UserEventFunc();
      virtual Int_t UserPostLoop();
   private:
      TGo4MbsEvent    *fRawEvent;
      THadaqUnpackEvent *fUnpackEvent;
      TH1D            *fSize;
      Int_t            fEvents;
      Int_t            fLastEvent;

   ClassDef(THadaqAnalysis,1)
};
#endif //THadaqANALYSIS_H



