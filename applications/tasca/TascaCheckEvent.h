//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef TascaCheckEVENT_H
#define TascaCheckEVENT_H

#include "TGo4EventElement.h"

class TascaCheckProc;

class TascaCheckEvent : public TGo4EventElement {
   public:
      TascaCheckEvent() ;
      TascaCheckEvent(const char* name) ;
      virtual ~TascaCheckEvent() ;
      virtual Int_t Init();
      virtual Int_t Fill();
      virtual void  Clear(Option_t *t=""){
	   memset((void*) &ffGammaKev[0], 0, sizeof(ffGammaKev));
	   fiGammaMysec=0;
	   fisEvr=kFALSE;
	   fisFission=kFALSE;
	   fisAlpha=kFALSE;
      }

      UInt_t fiEventNumber;
      Bool_t fisTof;
      Bool_t fisChopper;
      Bool_t fisMacro;
      Bool_t fisMicro;
      Bool_t fisVeto;
      Bool_t fisEvr;
      Bool_t fisFission;
      Bool_t fisAlpha;

      UInt_t fiSystemMysec;
      UInt_t fiTimeStamp;
      UInt_t fiGammaMysec;
      UInt_t fiDeltaSystemTime;
      UInt_t fiDeltaGammaTime;
      UInt_t fiDeltaTime;
      UInt_t fiGammaMulti;
      UInt_t fiAdcMulti;
      // index of maximum hit, if we had more than one hit
      UInt_t fiStopXLhitI;
      UInt_t fiStopXHhitI;
      UInt_t fiStopYLhitI;
      UInt_t fiStopYHhitI;
      UInt_t fiBackHhitI;
      UInt_t fiBackLhitI;
      UInt_t fiVetoHhitI;
      UInt_t fiVetoLhitI;
      UInt_t fiMultiStopXL;
      UInt_t fiMultiStopXH;
      UInt_t fiMultiStopYL;
      UInt_t fiMultiStopYH;
      UInt_t fiMultiBackH;
      UInt_t fiMultiBackL;
      UInt_t fiMultiVetoH;
      UInt_t fiMultiVetoL;
      UInt_t fiMultiGamma;
      UInt_t fiMultiAdc;
      // value of maximum hit, if we had more than one hit
      Float_t ffStopXLhitV;
      Float_t ffStopXHhitV;
      Float_t ffStopYLhitV;
      Float_t ffStopYHhitV;
      Float_t ffBackHhitV;
      Float_t ffBackLhitV;
      Float_t ffVetoHhitV;
      Float_t ffVetoLhitV;
      Float_t ffGammaMax;
      Float_t ffGammaSum;

      Float_t ffGammaKev[7];

   private:
      // this object is streamed. Therefore pointers must be excluded!
      // Otherwise these objects are streamed as well
      TascaCheckProc     * fxTascaCP;  //! Don't put this to file

   ClassDef(TascaCheckEvent,1)
};
#endif //TascaCheckEVENT_H



