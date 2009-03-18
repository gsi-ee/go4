#ifndef TMESHRAWEVENT_H
#define TMESHRAWEVENT_H

#include "TGo4EventElement.h"

#include "TMeshB1InputEvent.h"
#include "TMeshB2InputEvent.h"
#include "TMeshB3InputEvent.h"

class TMeshUnpackProc;
class TGo4FileSource;

class TMeshRawEvent : public TGo4EventElement {
   public:
      TMeshRawEvent() ;
      TMeshRawEvent(const char* name);
      virtual ~TMeshRawEvent() ;
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

       /** we compose data structure from subbranches: */
       TMeshB1InputEvent fxSub1;
       TMeshB2InputEvent fxSub2;
       TMeshB3InputEvent fxSub3;

   private:
      TMeshUnpackProc * fxUnpacker;  //! Don't put this to file
      TGo4FileSource * fxFile;  //! Don't put this to file

   ClassDef(TMeshRawEvent,1)
};
#endif //TMESHRAWEVENT_H



