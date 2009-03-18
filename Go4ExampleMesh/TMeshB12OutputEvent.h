#ifndef TMESHB12OUTPUTEVENT_H
#define TMESHB12OUTPUTEVENT_H

#include "TGo4EventElement.h"

class TMeshB12AnlProc;
class TGo4FileSource;

class TMeshB12OutputEvent : public TGo4EventElement {
   public:
      TMeshB12OutputEvent() ;
      TMeshB12OutputEvent(const char* name) ;
      virtual ~TMeshB12OutputEvent() ;
      virtual Int_t Init();
      virtual Int_t Fill();
      virtual void  Clear(Option_t *t="");

      Float_t frData[16];
   private:
      // this object is streamed. Therefore pointers must be excluded!
      // Otherwise these objects are streamed as well
      TMeshB12AnlProc     * fxProcessor;  //! Don't put this to file
      TGo4FileSource  * fxFile;  //! Don't put this to file

   ClassDef(TMeshB12OutputEvent,1)
};
#endif //TMESHB12OUTPUTEVENT_H



