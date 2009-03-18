#ifndef TMESHB2INPUTEVENT_H
#define TMESHB2INPUTEVENT_H

#include "TGo4EventElement.h"

class TMeshUnpackProc;
class TGo4FileSource;

class TMeshB2InputEvent : public TGo4EventElement {
   public:
      TMeshB2InputEvent() ;
      TMeshB2InputEvent(const char* name);
      virtual ~TMeshB2InputEvent() ;

      Int_t Fill();

      void Clear(Option_t *t="");
      Int_t Init();

      Int_t fiCrate2[16];
   private:
      TMeshUnpackProc * fxProcessor;  //! Don't put this to file
      TGo4FileSource * fxFile;  //! Don't put this to file

   ClassDef(TMeshB2InputEvent,1)
};

#endif //TMESHB2INPUTEVENT_H
