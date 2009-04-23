#ifndef TascaANALYSIS_H
#define TascaANALYSIS_H

#include "TGo4Analysis.h"
#include "TGo4Picture.h"

class TFile;
class TH1;
class TascaParameter;
class TGo4Picture;

class TascaAnalysis : public TGo4Analysis  {
   public:
            TascaAnalysis();
           ~TascaAnalysis() ;
      Int_t UserPreLoop();
      Int_t UserEventFunc();
      Int_t UserPostLoop();
      void SetPicture(TGo4Picture *p, TH1 *h, UInt_t ir, UInt_t ic, UInt_t mode);
      TGo4Picture* CreatePicture(const Text_t* folder, const Text_t* name, const Text_t* title, UInt_t ir, UInt_t ic);
      TH1I* CreateTH1I(const Text_t* folder, const Text_t* name, const Text_t* title, UInt_t channels, Double_t low, Double_t high);
   private:
	      TascaParameter      *fPar;

   ClassDef(TascaAnalysis,1)
};

#endif //TascaANALYSIS_H



