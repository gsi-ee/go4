//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef CompCaliEVENT_H
#define CompCaliEVENT_H

#include "TGo4EventElement.h"

class CompCaliProc;
class TGo4FileSource;

class CompCaliEvent : public TGo4EventElement {
   public:
      CompCaliEvent();
      CompCaliEvent(const char* name) ;
      virtual ~CompCaliEvent() ;
      /**
       * Method called by the event owner (analysis step) to fill the
       * event element from the set event source. Event source can
       * be the source of the analysis step (if this is a raw event)
       * or the event processor (if this is a reduced event).
       */
      Int_t Fill();

      /**
       * Method called by the event owner (analysis step) to clear the
       * event element.
       */
      void Clear(Option_t *t=""){
       fiEventNumber=0;
//	   memset((void*) &ffStopXL[0],0, sizeof(ffStopXL));
//	   memset((void*) &ffStopXH[0],0, sizeof(ffStopXH));
//	   memset((void*) &ffStopYL[0],0, sizeof(ffStopYL));
//	   memset((void*) &ffStopYH[0],0, sizeof(ffStopYH));
//	   memset((void*) &ffBackL[0], 0, sizeof(ffBackL));
//	   memset((void*) &ffBackH[0], 0, sizeof(ffBackH));
//	   memset((void*) &ffVetoL[0], 0, sizeof(ffVetoL));
//	   memset((void*) &ffVetoH[0], 0, sizeof(ffVetoH));
      }

      Int_t Init();
      void PrintEvent();

      UInt_t fiFileNumber;
      UInt_t fiEventNumber;
      Bool_t fisTof;
      Bool_t fisChopper;
      Bool_t fisMacro;
      Bool_t fisMicro;
      Bool_t fisVeto;

      UInt_t fiSystemMysec;
      UInt_t fiSystemmsec;
      UInt_t fiTimeStamp;
      UInt_t fiGammaMysec;
//      Float_t ffStopXL[144]; //! dont store
//      Float_t ffStopXH[144]; //! dont store
//      Float_t ffStopYL[96];  //! dont store
//      Float_t ffStopYH[96]; //! dont store
//      Float_t ffBackH[64];  //! dont store
//      Float_t ffBackL[64]; //! dont store
//      Float_t ffVetoH[16]; //! dont store
//      Float_t ffVetoL[16]; //! dont store
      Float_t ffGammaKev[7];
      Float_t ffGammaMax;
      Float_t ffGammaSum;
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
      UInt_t fiMultiStopYL;
      UInt_t fiMultiStopYH;
      UInt_t fiMultiBackH;
      UInt_t fiMultiBackL;
      UInt_t fiMultiVetoH;
      UInt_t fiMultiVetoL;
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

   private:
      CompCaliProc * fxCompEP;  //! Don't put this to file
      TGo4FileSource * fxCompFS;  //! Don't put this to file

   ClassDef(CompCaliEvent,1)
};
#endif //CompCaliEVENT_H



