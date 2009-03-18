#ifndef TMESHB2OUTPUTEVENT_H
#define TMESHB2OUTPUTEVENT_H

#include "TGo4EventElement.h"

class TMeshB2AnlProc;
class TGo4FileSource;

class TMeshB2OutputEvent : public TGo4EventElement {
   public:
      TMeshB2OutputEvent() ;
      TMeshB2OutputEvent(const char* name) ;
      virtual ~TMeshB2OutputEvent() ;
      virtual Int_t Init();
      virtual Int_t Fill();
      virtual void  Clear(Option_t *t="");

      Float_t frData[16];
   private:
      // this object is streamed. Therefore pointers must be excluded!
      // Otherwise these objects are streamed as well
      TMeshB2AnlProc     * fxProcessor;  //! Don't put this to file
      TGo4FileSource  * fxFile;  //! Don't put this to file

   ClassDef(TMeshB2OutputEvent,1)
};
#endif //TMESHB12OUTPUTEVENT_H



