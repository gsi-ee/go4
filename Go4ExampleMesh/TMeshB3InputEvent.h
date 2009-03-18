#ifndef TMESHB3INPUTEVENT_H
#define TMESHB3INPUTEVENT_H

#include "TGo4EventElement.h"

class TMeshUnpackProc;
class TGo4FileSource;

class TMeshB3InputEvent : public TGo4EventElement {
   public:
      TMeshB3InputEvent();
      TMeshB3InputEvent(const char* name) ;
      virtual ~TMeshB3InputEvent() ;
      Int_t Fill();
      void Clear(Option_t *t="");
      Int_t Init();

      Int_t fiCrate3[16];
   private:
      TMeshUnpackProc * fxProcessor;  //! Don't put this to file
      TGo4FileSource * fxFile;  //! Don't put this to file

   ClassDef(TMeshB3InputEvent,1)
};

#endif //TMESHB3INPUTEVENT_H
