#ifndef TascaANALYSIS_H
#define TascaANALYSIS_H

#include "TGo4Analysis.h"

class TFile;
class TH1D;
class TGo4MbsEvent;
class TascaUnpackEvent;
class TascaAnlEvent;
class TascaParameter;
class TGo4FitterEnvelope;

class TascaAnalysis : public TGo4Analysis  {
   public:
                     TascaAnalysis();
                     TascaAnalysis(Text_t * input, Int_t type, Int_t port
                                  , Text_t * out1, Text_t * out2, Bool_t en1, Bool_t en2);
      virtual       ~TascaAnalysis() ;
      virtual Int_t UserPreLoop();
      virtual Int_t UserEventFunc();
      virtual Int_t UserPostLoop();
   private:
      TFile              *fUserFile;
      TGo4MbsEvent       *fMbsEvent;
      TascaUnpackEvent    *fRawEvent;
      TascaAnlEvent       *fCalEvent;
      TascaParameter      *fPar;
      TGo4FitterEnvelope *fFitEnvSize;
      TGo4FitterEnvelope *fFitEnvSpectrum;
      TH1D               *fSize;
      Int_t               fEvents;
      Int_t               fLastEvent;

   ClassDef(TascaAnalysis,1)
};

#endif //TascaANALYSIS_H



