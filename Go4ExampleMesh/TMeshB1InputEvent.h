#ifndef TMESHB1INPUTEVENT_H
#define TMESHB1INPUTEVENT_H

#include "TGo4EventElement.h"

class TMeshUnpackProc;
class TGo4FileSource;

class TMeshB1InputEvent : public TGo4EventElement {
   public:
      TMeshB1InputEvent() ;
      TMeshB1InputEvent(const char* name) ;
      virtual ~TMeshB1InputEvent() ;
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
   private:
      TMeshUnpackProc * fxProcessor;  //! Don't put this to file
      TGo4FileSource * fxFile;  //! Don't put this to file

   ClassDef(TMeshB1InputEvent,1)
};

#endif //TMESHB1INPUTEVENT_H
