//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef CompCheckPROCESSOR_H
#define CompCheckPROCESSOR_H

#include "TGo4EventProcessor.h"

class CompControl;
class CompParameter;
class CompCheckEvent;
class CompUnpackEvent;
class CompCalibration;
class CompCaliFitter;
class CompAnalysis;

class CompCheckProc : public TGo4EventProcessor {
   public:
      CompCheckProc() ;
      CompCheckProc(const char * name);
      virtual ~CompCheckProc();

      void CompEventCheck(CompCheckEvent* target);


	  CompAnalysis  * anl;
      CompUnpackEvent *fInput;
      CompControl  *fControl;
      CompParameter *fParam;
      TGo4WinCond   *fadcKevL;
      TGo4WinCond   *fadcKevH;
      TGo4WinCond   *fgammaKev;
      TGo4WinCond   *fgammaMysec;
      TGo4WinCond   *fwinEvr;
      TGo4WinCond   *fwinAlpha;
      TGo4WinCond   *fwinFission1;
      TGo4WinCond   *fwinAlpha1;
      TGo4WinCond   *fwinAlpha2;
      TGo4WinCond   *fwinFission2;
      TGo4WinCond   *fwinBack;
      TH2D *fStopLE[48];
      TH2D *fStopHE[48];
      TH2D *fStopXY;
      TH1I	*fTime;
      UInt_t TimeLastmysec;
      UInt_t TimeLastadc;
      UInt_t i,ii,k;
      UInt_t fiEventsProcessed;
      UInt_t fiEventsWritten;
      Int_t fLastEvent;

      TH1I *fhTimeStamp;
      TH1I *fhStopXL[144];  // 6x 8x6
      TH1I *fhStopXH[144];
      TH1I *fhStopYL[96];  // 4x 8x6
      TH1I *fhStopYH[96];
      TH1D *fhdStopXL;
      TH1D *fhdStopXH;
      TH1D *fhdStopYL;
      TH1D *fhdStopYH;
      TH1D *fhdStopYLsum;
      TH1D *fhdStopYHsum;
      TH1D *fhdStopXHsum;
      TH1D *fhdStopXLsum;
	  CompCalibration 	*fCalibration;
	  CompCaliFitter 	*fCaliFit;
      TGraph* 		fCaliGraph;
      TH2I  *fhStopH;
      TH2I  *fhStopL;
      TH2I  *fhStopXLH;
      TH2I  *fhStopYLH;
      TGo4Picture   *StopX[6];
      TGo4Picture   *StopY[4];
      TGo4Picture   *Sum;
      TGo4Picture   *Hits;
   ClassDef(CompCheckProc,1)
};
#endif //CompCheckPROCESSOR_H
