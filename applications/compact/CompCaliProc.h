//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef CompCaliPROCESSOR_H
#define CompCaliPROCESSOR_H

#include "TGo4EventProcessor.h"
#include "CompCodec.h"

class TGraph;
class CompControl;
class CompParameter;
class CompCalibration;
class CompCaliFitter;
class CompUnpackEvent;
class CompCaliEvent;
class CompAnalysis;

class CompCaliProc : public TGo4EventProcessor {
   public:
      CompCaliProc();
      CompCaliProc(const char* name);
     ~CompCaliProc() ;
      void CompCalibrate(CompCaliEvent* target);
  private:
	  CompAnalysis * anl;
      TGo4WinCond   *fadcKevL;
      TGo4WinCond   *fadcKevH;
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
      TH1D *fhdStopYLsum;
      TH1D *fhdStopYHsum;
      TH1D *fhdStopXHsum;
      TH1D *fhdStopXLsum;
      TH1D *fhdBackH;
      TH1D *fhdBackL;
      TH1D *fhdVetoH;
      TH1D *fhdVetoL;
      TH1D *fhdBackHsum;
      TH1D *fhdBackLsum;
      TH1D *fhdVetoHsum;
      TH1D *fhdVetoLsum;
      TH1I *fhGammaMysec;
      TH1I *fhGamma10ns[7];
      TH1I *fhGammaKev[7];
      TH1I *fhGammaSumKev;
      TH1I *fhGammaAddbackKev;
      CompControl  *fControl;
	  CompCalibration 	*fCalibration;
	  CompCaliFitter 	*fCaliFit;
      CompParameter *fParam;
      TGraph* 		fCaliGraph;
      TH2I  *fhStopH;
      TH2I  *fhStopL;
      TH2I  *fhStopXLH;
      TH2I  *fhStopYLH;
      CompUnpackEvent  *fInput;
      TGo4Picture   *StopX[6];
      TGo4Picture   *StopY[4];
      TGo4Picture   *Back[2];
      TGo4Picture   *Veto;
      TGo4Picture   *Gamma;
      TGo4Picture   *Sum;
      TGo4Picture   *Hits;
      UInt_t i,k,n;
      UInt_t fLastEvent;
      UInt_t fiEventsProcessed;
      UInt_t fiEventsWritten;

   ClassDef(CompCaliProc,1)
};

#endif //CompCaliPROCESSOR_H
