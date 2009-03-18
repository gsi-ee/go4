#ifndef TEVENT_H
#define TEVENT_H

#include "TGo4EventElement.h"

class TXXXEvent : public TGo4EventElement {
   public:
      TXXXEvent();
      TXXXEvent(const char* name);
      virtual ~TXXXEvent();

      /** Method called by the framework to clear the event element. */
      void Clear(Option_t *t="");

      Float_t fCrate1[8];
      Float_t fCrate2[8];

   ClassDef(TXXXEvent,1)
};
#endif //TEVENT_H



