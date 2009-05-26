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

   	   memset((void*) &fiStopXLhits[0],0, sizeof(fiStopXLhits));
	   memset((void*) &fiStopXHhits[0],0, sizeof(fiStopXHhits));
	   memset((void*) &fiStopYLhits[0],0, sizeof(fiStopYLhits));
	   memset((void*) &fiStopYHhits[0],0, sizeof(fiStopYHhits));
	   memset((void*) &fiBackLhits[0], 0, sizeof(fiBackLhits));
	   memset((void*) &fiBackHhits[0], 0, sizeof(fiBackHhits));
	   memset((void*) &fiVetoLhits[0], 0, sizeof(fiVetoLhits));
	   memset((void*) &fiVetoHhits[0], 0, sizeof(fiVetoHhits));

	   memset((void*) &fiGammaQ[0], 0, sizeof(fiGammaQ));
	   memset((void*) &fiGammaE[0], 0, sizeof(fiGammaE));
	   memset((void*) &fiGammaT[0], 0, sizeof(fiGammaT));

	   fiStopXLhitI=0;
	   fiStopXHhitI=0;
	   fiStopYLhitI=0;
	   fiStopYHhitI=0;
	   fiBackHhitI=0;
	   fiBackLhitI=0;
	   fiVetoHhitI=0;
	   fiVetoLhitI=0;
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
      UInt_t fiSystemSec;
      UInt_t fiSystemMysec;
      UInt_t fiTimeStamp;
      // Data fields of detectors
      UInt_t fiStopXL[144];
      UInt_t fiStopXH[144];
      UInt_t fiStopYL[96];
      UInt_t fiStopYH[96];
      UInt_t fiBackH[64];
      UInt_t fiBackL[64];
      UInt_t fiVetoH[16];
      UInt_t fiVetoL[16];
      // hit list of detectors (indices of first > 0)
      UInt_t fiStopXLhits[4];
      UInt_t fiStopXHhits[4];
      UInt_t fiStopYLhits[4];
      UInt_t fiStopYHhits[4];
      UInt_t fiBackHhits[4];
      UInt_t fiBackLhits[4];
      UInt_t fiVetoHhits[4];
      UInt_t fiVetoLhits[4];
      // index of maximum hit, if we had more than one hit
      UInt_t fiStopXLhitI;
      UInt_t fiStopXHhitI;
      UInt_t fiStopYLhitI;
      UInt_t fiStopYHhitI;
      UInt_t fiBackHhitI;
      UInt_t fiBackLhitI;
      UInt_t fiVetoHhitI;
      UInt_t fiVetoLhitI;
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
      UInt_t fiGammaT[8];
      UInt_t fiGammaE[8];
      UInt_t fiGammaQ[8];

      Bool_t fisTof;
      Bool_t fisChopper;
      Bool_t fisMacro;
      Bool_t fisMicro;

   private:
      TascaUnpackProc * fxTascaEP;  //! Don't put this to file
      TGo4FileSource * fxTascaFS;  //! Don't put this to file

   ClassDef(TascaUnpackEvent,1)
};
#endif //TascaEVENT_H



