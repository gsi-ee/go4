#ifndef TMESHB3OUTPUTEVENT_H
#define TMESHB3OUTPUTEVENT_H

#include "TGo4EventElement.h"

class TMeshB3AnlProc;
class TGo4FileSource;

class TMeshB3OutputEvent : public TGo4EventElement {
   public:
      TMeshB3OutputEvent();
      TMeshB3OutputEvent(const char* name);
      virtual ~TMeshB3OutputEvent();
      virtual Int_t Init();
      virtual Int_t Fill();
      virtual void  Clear(Option_t *t="");

      Float_t frData[16];
   private:
      // this object is streamed. Therefore pointers must be excluded!
      // Otherwise these objects are streamed as well
      TMeshB3AnlProc     * fxProcessor;  //! Don't put this to file
      TGo4FileSource  * fxFile;  //! Don't put this to file

   ClassDef(TMeshB3OutputEvent,1)
};

#endif //TMESHB3OUTPUTEVENT_H
