#ifndef TMESHDUMMYEVENT_H
#define TMESHDUMMYEVENT_H

#include "TGo4EventElement.h"

class TMeshProviderProc;

/** The only purpose for this class is to trigger the method of
* the corresponding provider processor to get the correct event
* pointer from framework. */

class TMeshDummyEvent : public TGo4EventElement {
   public:
      TMeshDummyEvent() ;
      TMeshDummyEvent(const char* name) ;
      virtual ~TMeshDummyEvent() ;

      virtual Int_t Fill();
      virtual void  Clear(Option_t *t="");
      virtual Int_t Init();

   private:
      TMeshProviderProc* fxProviderProc;//!

   ClassDef(TMeshDummyEvent,1)
};

#endif //TMESHDUMMYANLEVENT_H
