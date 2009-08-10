//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef CompUnpackEVENT_H
#define CompUnpackEVENT_H

#include "TGo4EventElement.h"

class CompUnpackProc;
class TGo4FileSource;

class CompUnpackEvent : public TGo4EventElement {
   public:
      CompUnpackEvent();
      CompUnpackEvent(const char* name) ;
      virtual ~CompUnpackEvent() ;
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
       fiFileNumber=0;
       fiEventNumber=0;
	   fiMultiAdc=0;
	   fiStopXLhitI=-1;
	   fiStopXHhitI=-1;
	   fiStopYLhitI=-1;
	   fiStopYHhitI=-1;
	   fiStopXLhitV=0;
	   fiStopXHhitV=0;
	   fiStopYLhitV=0;
	   fiStopYHhitV=0;
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
      // Data fields of detectors
      UInt_t fiStopXL[144];
      UInt_t fiStopXH[144];
      UInt_t fiStopYL[96];
      UInt_t fiStopYH[96];
      // index of maximum hit, if we had more than one hit
      Int_t fiStopXLhitI;
      Int_t fiStopXHhitI;
      Int_t fiStopYLhitI;
      Int_t fiStopYHhitI;
     UInt_t fiMultiStopXL;
      UInt_t fiMultiStopXH;
      UInt_t fiMultiStopYL;
      UInt_t fiMultiStopYH;
      UInt_t fiMultiAdc;
      // value of maximum hit, if we had more than one hit
      UInt_t fiStopXLhitV;
      UInt_t fiStopXHhitV;
      UInt_t fiStopYLhitV;
      UInt_t fiStopYHhitV;

      UInt_t fiMpxi[40];

   private:
      CompUnpackProc * fxCompEP;  //! Don't put this to file
      TGo4FileSource * fxCompFS;  //! Don't put this to file

   ClassDef(CompUnpackEvent,1)
};
#endif //CompEVENT_H



