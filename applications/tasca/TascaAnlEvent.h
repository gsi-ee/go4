#ifndef TascaANLEVENT_H
#define TascaANLEVENT_H

#include "TGo4EventElement.h"

class TascaAnlProc;

class TascaAnlEvent : public TGo4EventElement {
   public:
      TascaAnlEvent() ;
      TascaAnlEvent(const char* name) ;
      virtual ~TascaAnlEvent() ;
      virtual Int_t Init();
      virtual Int_t Fill();
      virtual void  Clear(Option_t *t="");

      Float_t frData[16];
   private:
      // this object is streamed. Therefore pointers must be excluded!
      // Otherwise these objects are streamed as well
      TascaAnlProc     * fxTascaCP;  //! Don't put this to file

   ClassDef(TascaAnlEvent,1)
};
#endif //TascaANLEVENT_H



