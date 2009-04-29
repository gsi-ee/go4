#ifndef TascaUNPACKPROCESSOR_H
#define TascaUNPACKPROCESSOR_H

#include "TGo4EventProcessor.h"
#include "TascaCodec.h"

class TascaParameter;
class TascaPedestals;
class TascaUnpackEvent;
class TascaAnalysis;

class TascaUnpackProc : public TGo4EventProcessor {
   public:
      TascaUnpackProc();
      TascaUnpackProc(const char* name);
      virtual ~TascaUnpackProc() ;
      void TascaUnpack(TascaUnpackEvent* target);
      Bool_t DecodeGamma(UInt_t* data, UInt_t* behind);
  private:
	  void CalcPedestals();

	  TascaAnalysis * anl;
      TascaPedestals *fPedestals;
      TascaParameter *fParam;
	  TascaCodec 	*codec;
      TH1I          *fAdc[96];
      TH1I  		*fPedestal;
      TH1I  		*fContent;
      TH1I			*fTree;
      TH1I			*fAdcAllRaw;
      TH1I			*fAdcAllCal;
      TH1I			*fGammaE[8];
      TH1I			*fGammaT[8];
      TH1I          *fTrace[8];
      TH1I          *fTrace_e[8];
      TH1I          *fHisto[8];
      TH1I          *fPileup[8];
      TascaUnpackEvent* pUnpackEvent;
      TGo4MbsEvent  *fInput;
      TGo4Picture   *Geraw;
      TGo4Picture   *M1raw;
      TGo4Picture   *M2raw;
      TGo4Picture   *M3raw;
      UInt_t i,k,n, evcount;

   ClassDef(TascaUnpackProc,1)
};

#endif //TascaUNPACKPROCESSOR_H
