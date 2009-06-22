//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef TascaUnpackEVENT_H
#define TascaUnpackEVENT_H

#include "TGo4EventElement.h"

class TascaUnpackProc;
class TGo4FileSource;

class TascaUnpackEvent : public TGo4EventElement {
   public:
      TascaUnpackEvent();
      TascaUnpackEvent(const char* name) ;
      virtual ~TascaUnpackEvent() ;
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
   	   memset((void*) &fiAdc[0],  0, sizeof(fiAdc));
   	   memset((void*) &fiStopXL[0],0, sizeof(fiStopXL));
   	   memset((void*) &fiStopXH[0],0, sizeof(fiStopXH));
   	   memset((void*) &fiStopYL[0],0, sizeof(fiStopYL));
   	   memset((void*) &fiStopYH[0],0, sizeof(fiStopYH));
   	   memset((void*) &fiBackL[0], 0, sizeof(fiBackL));
   	   memset((void*) &fiBackH[0], 0, sizeof(fiBackH));
   	   memset((void*) &fiVetoL[0], 0, sizeof(fiVetoL));
   	   memset((void*) &fiVetoH[0], 0, sizeof(fiVetoH));

	   memset((void*) &fiGammaQ[0], 0, sizeof(fiGammaQ));
	   memset((void*) &fiGammaE[0], 0, sizeof(fiGammaE));
	   memset((void*) &fiGammaChannelTime[0], 0, sizeof(fiGammaChannelTime));
       fiFileNumber=0;
       fiEventNumber=0;
	   fiGammaTime=0;
	   fiTimeStamp=0;
	   fiMultiGamma=0;
	   fiMultiAdc=0;
	   fiStopXLhitI=-1;
	   fiStopXHhitI=-1;
	   fiStopYLhitI=-1;
	   fiStopYHhitI=-1;
	   fiBackHhitI=-1;
	   fiBackLhitI=-1;
	   fiVetoHhitI=-1;
	   fiVetoLhitI=-1;
	   fiStopXLhitV=0;
	   fiStopXHhitV=0;
	   fiStopYLhitV=0;
	   fiStopYHhitV=0;
	   fiBackHhitV=0;
	   fiBackLhitV=0;
	   fiVetoHhitV=0;
	   fiVetoLhitV=0;
	   fiSystemSec=0;
	   fiSystemMysec=0;
	   fiTimeStamp=0;
      }

      Int_t Init();

      UInt_t fiAdc[96];  //! Don't put this to file
      UInt_t fiFileNumber;
      UInt_t fiEventNumber;
      Bool_t fisTof;
      Bool_t fisChopper;
      Bool_t fisMacro;
      Bool_t fisMicro;
      Bool_t fisVeto;

      UInt_t fiSystemSec;
      UInt_t fiSystemMysec;
      UInt_t fiTimeStamp;
      UInt_t fiGammaTime;
      UInt_t fiGammaChannelTime[7];
      // Data fields of detectors
      UInt_t fiStopXL[144];
      UInt_t fiStopXH[144];
      UInt_t fiStopYL[96];
      UInt_t fiStopYH[96];
      UInt_t fiBackH[64];
      UInt_t fiBackL[64];
      UInt_t fiVetoH[16];
      UInt_t fiVetoL[16];
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
      UInt_t fiMultiAdc;
      // value of maximum hit, if we had more than one hit
      UInt_t fiStopXLhitV;
      UInt_t fiStopXHhitV;
      UInt_t fiStopYLhitV;
      UInt_t fiStopYHhitV;
      UInt_t fiBackHhitV;
      UInt_t fiBackLhitV;
      UInt_t fiVetoHhitV;
      UInt_t fiVetoLhitV;

      UInt_t fiMpxi[40];
      UInt_t fiGammaE[7];
      UInt_t fiGammaQ[8];

   private:
      TascaUnpackProc * fxTascaEP;  //! Don't put this to file
      TGo4FileSource * fxTascaFS;  //! Don't put this to file

   ClassDef(TascaUnpackEvent,1)
};
#endif //TascaEVENT_H



