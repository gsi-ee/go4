#ifndef TascaEVENT_H
#define TascaEVENT_H

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
      void Clear(Option_t *t="");
      Int_t Init();

      UInt_t fiAdc[96];  //! Don't put this to file
      UInt_t fiStopXL[144];
      UInt_t fiStopXH[144];
      UInt_t fiStopYL[96];
      UInt_t fiStopYH[96];
      UInt_t fiBackH[64];
      UInt_t fiBackL[64];
      UInt_t fiVetoH[16];
      UInt_t fiVetoL[16];

   private:
      TascaUnpackProc * fxTascaEP;  //! Don't put this to file
      TGo4FileSource * fxTascaFS;  //! Don't put this to file

   ClassDef(TascaUnpackEvent,1)
};
#endif //TascaEVENT_H



