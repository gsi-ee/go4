#ifndef TXXXANALYSIS_H
#define TXXXANALYSIS_H

#include "TGo4Analysis.h"

class TFile;
class TH1D;
class TGo4MbsEvent;
class TXXXUnpackEvent;
class TXXXAnlEvent;
class TXXXParameter;
class TGo4FitterEnvelope;

class TXXXAnalysis : public TGo4Analysis  {
   public:
                     TXXXAnalysis();
                     TXXXAnalysis(Text_t * input, Int_t type, Int_t port
                                  , Text_t * out1, Text_t * out2, Bool_t en1, Bool_t en2);
      virtual       ~TXXXAnalysis() ;
      virtual Int_t UserPreLoop();
      virtual Int_t UserEventFunc();
      virtual Int_t UserPostLoop();
   private:
      TFile              *fUserFile;
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



