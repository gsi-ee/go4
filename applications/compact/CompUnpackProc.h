//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef CompUNPACKPROCESSOR_H
#define CompUNPACKPROCESSOR_H

#include "TGo4EventProcessor.h"
#include "CompCodec.h"

class CompControl;
class CompParameter;
class CompPedestals;
class CompUnpackEvent;
class CompAnalysis;

class CompUnpackProc : public TGo4EventProcessor {
 public:
  CompUnpackProc();
  CompUnpackProc(const char* name);
  virtual ~CompUnpackProc() ;
  void CompUnpack(CompUnpackEvent* target);
  Bool_t DecodeGamma(UInt_t* data, UInt_t* behind);
 private:
  void CalcPedestals();

  CompAnalysis * anl;
  CompPedestals *fPedestals;
  CompControl  *fControl;
  CompParameter *fParam;
  CompCodec 	*codec;
  TH2I *fhStopXLH;
  TH2I *fhStopYLH;
  TH2I *fhStopXiLH;
  TH2I *fhStopYiLH;
  TH1I *fAdc[96];
  TH1I *fSpill;
  TH1I *fSpillG;
  TH1I *fTof;
  TH1I *fTofgated;
  TH1I *fTest;
  TH1I *fSize;
  TH1I *fSizeA;
  TH1I *fSizeG;
  TH1I *fPedestal;
  TH1I *fContent;
  TH1I *fTree;
  TH1I *fTime;
  TH1I *fAdcAllRaw;
  TH1I *fFilter;
  TH1I *fGammaE[7];
  TH1I *fGammaT[7];
  TH1I *fGammaTime;
  TH1I *fSystemTime;
  TH1I *fAdcTime;
  TH1I *fMaxiStopXLg;
  TH1I *fMaxiStopXHg;
  TH1I *fMaxiStopYLg;
  TH1I *fMaxiStopYHg;
  TH1I *fMultiGamma;
  TH1I *fMultiAdc;
  TH1I *fMultiStopXL;
  TH1I *fMultiStopXH;
  TH1I *fMultiStopYL;
  TH1I *fMultiStopYH;
  TH1I *fMultiBackH;
  TH1I *fMultiBackL;
  TH1I *fMultiVetoL;
  TH1I *fMultiVetoH;
  TH1I *fTrace[8];
  TH1I *fTrace_e[8];
  TH1I *fHisto[8];
  TH1I *fPileup[8];
  CompUnpackEvent* pUnpackEvent;
  TGo4MbsEvent  *fInput;
  TGo4Picture   *Geraw;
  TGo4Picture   *Multi;
  TGo4Picture   *M1raw;
  TGo4Picture   *M2raw;
  TGo4Picture   *M3raw;
  ULong64_t	fGammaStamps[8];
  UInt_t i,k,n;
  UInt_t fiEventsProcessed;
  UInt_t fiEventsWritten;
  UInt_t fiDeltaSystemTime;
  UInt_t fiDeltaGammaTime;
  UInt_t fiDeltaTime;
  UInt_t TimeLastgamma;
  UInt_t TimeLastsec;
  UInt_t TimeLastmysec;
  UInt_t TimeLastadc;
  UInt_t fLastEvent;
  UInt_t spillTime0;
  UInt_t spillIndex;
  Int_t spillBins[100];
  Bool_t spillOn;
  Bool_t spillTest;
  UInt_t crate, address, channels, header, off;
  UInt_t lwords, multiL, multiH, low;
  UInt_t *pdata,*pbehind,*psubevent;
  UInt_t latches;
  UInt_t adcs;
  UInt_t patt0,patt1,patt2,patt3;
  UInt_t timestamp,lat0,lat1,lat2,lat3;
  UInt_t timediff;
  Bool_t takeEvent;
  char  cfilenum[8];
  char  cfilename[512];
  UInt_t filenum;
  UInt_t totalmsec;
  UInt_t lastmsec;
  UInt_t firstmsec;
  UInt_t lastfilenum;
  UInt_t msec;

  ClassDef(CompUnpackProc,1)
    };

#endif //CompUNPACKPROCESSOR_H
