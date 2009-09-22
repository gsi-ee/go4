#ifndef TXXXANLEVENT_H
#define TXXXANLEVENT_H

#include "TGo4EventElement.h"

class TXXXAnlEvent : public TGo4EventElement {
   public:
      TXXXAnlEvent() ;
      TXXXAnlEvent(const char* name) ;
      virtual ~TXXXAnlEvent() ;
      virtual void  Clear(Option_t *t="");

      Float_t frData[16];

   ClassDef(TXXXAnlEvent,1)
};
#endif //TXXXANLEVENT_H



