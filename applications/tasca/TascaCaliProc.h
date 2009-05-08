#ifndef TascaCaliPROCESSOR_H
#define TascaCaliPROCESSOR_H

#include "TGo4EventProcessor.h"
#include "TascaCodec.h"

class TGraph;
class TascaControl;
class TascaParameter;
class TascaCalibration;
class TascaUnpackEvent;
class TascaCaliEvent;
class TascaAnalysis;

class TascaCaliProc : public TGo4EventProcessor {
   public:
      TascaCaliProc();
      TascaCaliProc(const char* name);
     ~TascaCaliProc() ;
      void TascaCalibrate(TascaCaliEvent* target);
  private:
	  TascaAnalysis * anl;
      TH1I *fhTimeStamp;
      TH1I *fhStopXL[144];  // 6x 8x6
      TH1I *fhStopXH[144];
      TH1I *fhStopYL[96];  // 4x 8x6
      TH1I *fhStopYH[96];
      TH1I *fhBackH[64]; // 8x8
      TH1I *fhBackL[64];
      TH1I *fhVetoH[16]; // 8x6
      TH1I *fhVetoL[16];
      TH1D *fhdStopXL;
      TH1D *fhdStopXH;
      TH1D *fhdStopYL;
      TH1D *fhdStopYH;
      TH1D *fhdBackH;
      TH1D *fhdBackL;
      TH1D *fhdVetoH;
      TH1D *fhdVetoL;
      TH1I *fhGammaMysec[8];
      TH1I *fhGammaKev[8];
      TascaControl  *fControl;
	  TascaCalibration 	*fCalibration;
      TH2I          *fhStopH;
      TH2I          *fhStopL;
      TGraph* 		fCaligraph;
      TascaUnpackEvent  *fInput;
      TGo4Picture   *StopX[6];
      TGo4Picture   *StopY[4];
      TGo4Picture   *Back[2];
      TGo4Picture   *Veto;
      TGo4Picture   *Gamma;
      TGo4Picture   *Sum;
      UInt_t i,k,n, evcount;

   ClassDef(TascaCaliProc,1)
};

#endif //TascaCaliPROCESSOR_H
