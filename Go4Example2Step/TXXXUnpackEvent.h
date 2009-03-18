#ifndef TXXXEVENT_H
#define TXXXEVENT_H

#include "TGo4EventElement.h"

class TXXXUnpackProc;
class TGo4FileSource;

class TXXXUnpackEvent : public TGo4EventElement {
   public:
      TXXXUnpackEvent();
      TXXXUnpackEvent(const char* name) ;
      virtual ~TXXXUnpackEvent() ;
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

      Int_t fiCrate1[16];
      Int_t fiCrate2[16];
      Int_t fiCrate3[16];
      Int_t fiCrate4[16];
   private:
      TXXXUnpackProc * fxXXXEP;  //! Don't put this to file
      TGo4FileSource * fxXXXFS;  //! Don't put this to file

   ClassDef(TXXXUnpackEvent,1)
};
#endif //TXXXEVENT_H



