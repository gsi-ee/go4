#ifndef TXXXANLEVENT_H
#define TXXXANLEVENT_H

#include "TGo4EventElement.h"

class TXXXAnlProc;

class TXXXAnlEvent : public TGo4EventElement {
   public:
      TXXXAnlEvent() ;
      TXXXAnlEvent(const char* name) ;
      virtual ~TXXXAnlEvent() ;
      virtual Int_t Init();
      virtual Int_t Fill();
      virtual void  Clear(Option_t *t="");

      Float_t frData[16];
   private:
      // this object is streamed. Therefore pointers must be excluded!
      // Otherwise these objects are streamed as well
      TXXXAnlProc     * fxXXXCP;  //! Don't put this to file

   ClassDef(TXXXAnlEvent,1)
};
#endif //TXXXANLEVENT_H



