//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef TascaUNPACKPROCESSOR_H
#define TascaUNPACKPROCESSOR_H

#include "TGo4EventProcessor.h"
#include "TascaCodec.h"

class TascaControl;
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
      TascaControl  *fControl;
      TascaParameter *fParam;
	  TascaCodec 	*codec;
      TH1I          *fAdc[96];
      TH1I  		*fSpill;
      TH1I  		*fSpillG;
      TH1I  		*fTof;
      TH1I  		*fTofgated;
      TH1I  		*fTest;
      TH1I  		*fSize;
      TH1I  		*fSizeA;
      TH1I  		*fSizeG;
      TH1I  		*fPedestal;
      TH1I  		*fContent;
      TH1I			*fTree;
      TH1I			*fTime;
      TH1I			*fAdcAllRaw;
      TH1I			*fFilter;
      TH1I			*fGammaE[7];
      TH1I			*fGammaT[7];
      TH1I			*fGammaTime;
      TH1I			*fSystemTime;
      TH1I			*fAdcTime;
      TH1I			*fMultiGamma;
      TH1I			*fMultiAdc;
      TH1I			*fMultiStopXL;
      TH1I			*fMultiStopXH;
      TH1I			*fMultiStopYL;
      TH1I			*fMultiStopYH;
      TH1I			*fMultiBackH;
      TH1I			*fMultiBackL;
      TH1I			*fMultiVetoL;
      TH1I			*fMultiVetoH;
      TH1I          *fTrace[8];
      TH1I          *fTrace_e[8];
      TH1I          *fHisto[8];
      TH1I          *fPileup[8];
      TascaUnpackEvent* pUnpackEvent;
      TGo4MbsEvent  *fInput;
      TGo4Picture   *Geraw;
      TGo4Picture   *Multi;
      TGo4Picture   *M1raw;
      TGo4Picture   *M2raw;
      TGo4Picture   *M3raw;
      ULong64_t	fGammaStamps[8];
      UInt_t i,k,n, evcount;
      UInt_t fiDeltaSystemTime;
      UInt_t fiDeltaGammaTime;
      UInt_t fiDeltaTime;
      UInt_t TimeLastgamma;
      UInt_t TimeLastsec;
      UInt_t TimeLastmysec;
      UInt_t TimeLastadc;
      UInt_t LastEvent;
      UInt_t spillTime0;
      UInt_t spillIndex;
      Int_t spillBins[100];
      Bool_t spillOn;
      Bool_t spillTest;
      Double_t StopXLcross[5];
      Double_t StopXHcross[5];
      Double_t StopYLcross[5];
      Double_t StopYHcross[5];
      Double_t BackLcross[5];
      Double_t BackHcross[5];
      Double_t VetoLcross[5];
      Double_t VetoHcross[5];
      UInt_t crate, address, channels, header, off;
      UInt_t lwords, multiL, multiH, low;
      UInt_t *pdata,*pbehind,*psubevent;
      UInt_t latches;
      UInt_t adcs;
      UInt_t patt0,patt1,patt2,patt3;
      UInt_t timestamp,lat0,lat1,lat2,lat3;
      UInt_t timediff;
      Bool_t takeEvent;

   ClassDef(TascaUnpackProc,1)
};

#endif //TascaUNPACKPROCESSOR_H
