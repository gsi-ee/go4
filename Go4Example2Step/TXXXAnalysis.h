#ifndef TXXXANALYSIS_H
#define TXXXANALYSIS_H

#include "TGo4Analysis.h"

class TH1D;
class TGo4MbsEvent;
class TXXXUnpackEvent;
class TXXXAnlEvent;
class TXXXParameter;
class TGo4FitterEnvelope;

class TXXXAnalysis : public TGo4Analysis  {
   public:
      TXXXAnalysis();
      TXXXAnalysis(int argc, char** argv);
      virtual ~TXXXAnalysis() ;
      virtual Int_t UserPreLoop();
      virtual Int_t UserEventFunc();
      virtual Int_t UserPostLoop();
   private:
      TGo4MbsEvent       *fMbsEvent;
      TXXXUnpackEvent    *fRawEvent;
      TXXXAnlEvent       *fCalEvent;
      TXXXParameter      *fPar;
      TGo4FitterEnvelope *fFitEnvSize;
      TGo4FitterEnvelope *fFitEnvSpectrum;
      TH1D               *fSize;
      Int_t               fEvents;
      Int_t               fLastEvent;

   ClassDef(TXXXAnalysis,1)
};

#endif //TXXXANALYSIS_H



