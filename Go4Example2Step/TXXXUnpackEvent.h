#ifndef TXXXEVENT_H
#define TXXXEVENT_H

#include "TGo4EventElement.h"

class TXXXUnpackEvent : public TGo4EventElement {
   public:
      TXXXUnpackEvent();
      TXXXUnpackEvent(const char* name) ;
      virtual ~TXXXUnpackEvent() ;

      /**
       * Method called by the event owner (analysis step) to clear the
       * event element.
       */
      void Clear(Option_t *t="");

      Int_t fiCrate1[16];
      Int_t fiCrate2[16];
      Int_t fiCrate3[16];
      Int_t fiCrate4[16];

   ClassDef(TXXXUnpackEvent,1)
};
#endif //TXXXEVENT_H



