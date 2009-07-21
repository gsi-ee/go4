//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef CompCheckEVENT_H
#define CompCheckEVENT_H

#include "TGo4EventElement.h"

class CompCheckProc;
class TGo4FileSource;
class CompEvent;

class CompCheckEvent : public TGo4EventElement {
   public:
      CompCheckEvent() ;
      CompCheckEvent(const char* name) ;
      virtual ~CompCheckEvent() ;
      virtual Int_t Init();
      virtual Int_t Fill();
      void CopyTo(CompEvent *t);
      void PrintEvent();
      virtual void  Clear(Option_t *t=""){
       fiEventNumber=0;
	   fisEvr=kFALSE;
	   fisFission=kFALSE;
	   fisAlpha=kFALSE;
     }

      UInt_t fiFileNumber;
      UInt_t fiEventNumber;
      Bool_t fisTof;
      Bool_t fisChopper;
      Bool_t fisMacro;
      Bool_t fisMicro;
      Bool_t fisVeto;
      Bool_t fisEvr;
      Bool_t fisFission;
      Bool_t fisAlpha;

      UInt_t fiSystemmsec;
      UInt_t fiSystemMysec;
      UInt_t fiTimeStamp;
      UInt_t fiGammaMysec;
      UInt_t fiDeltaSystemTime;
      UInt_t fiDeltaGammaTime;
      UInt_t fiDeltaTime;
      // index of maximum hit, if we had more than one hit
      Int_t fiStopXLhitI;
      Int_t fiStopXHhitI;
      Int_t fiStopYLhitI;
      Int_t fiStopYHhitI;
      Int_t fiBackHhitI;
      Int_t fiBackLhitI;
      Int_t fiVetoHhitI;
      Int_t fiVetoLhitI;
       UInt_t fiMultiStopXL;
       UInt_t fiMultiStopXH;
/*       UInt_t fiMultiStopYL; */
/*       UInt_t fiMultiStopYH; */
/*       UInt_t fiMultiBackH; */
/*       UInt_t fiMultiBackL; */
/*       UInt_t fiMultiVetoH; */
/*       UInt_t fiMultiVetoL; */
      UInt_t fiMultiGamma;
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

/*       Float_t ffGammaKev[7]; */

   private:
      // this object is streamed. Therefore pointers must be excluded!
      // Otherwise these objects are streamed as well
      CompCheckProc * fxCompCP;  //! Don't put this to file
      TGo4FileSource * fxCompFS;  //! Don't put this to file

   ClassDef(CompCheckEvent,1)
};
#endif //CompCheckEVENT_H



