#ifndef TMESHB1OUTPUTEVENT_H
#define TMESHB1OUTPUTEVENT_H

#include "TGo4EventElement.h"

class TMeshB1AnlProc;
class TGo4FileSource;

class TMeshB1OutputEvent : public TGo4EventElement {
   public:
      TMeshB1OutputEvent() ;
      TMeshB1OutputEvent(const char* name) ;
      virtual ~TMeshB1OutputEvent() ;
      virtual Int_t Init();
      virtual Int_t Fill();
      virtual void  Clear(Option_t *t="");

      Float_t frData[16];
   private:
      // this object is streamed. Therefore pointers must be excluded!
      // Otherwise these objects are streamed as well
      TMeshB1AnlProc     * fxProcessor;  //! Don't put this to file
      TGo4FileSource  * fxFile;  //! Don't put this to file

   ClassDef(TMeshB1OutputEvent,1)
};
#endif //TMESHB1OUTPUTEVENT_H



