#ifndef TMESHB3ANLPROCESSOR_H
#define TMESHB3ANLPROCESSOR_H

#include "TGo4EventProcessor.h"

class TMeshB3InputEvent;
class TMeshB3OutputEvent;

class TMeshB3AnlProc : public TGo4EventProcessor {

   public:
      TMeshB3AnlProc() ;
      TMeshB3AnlProc(const char* name);
      void Analysis(TMeshB3OutputEvent* target);
      virtual ~TMeshB3AnlProc() ;

      TMeshB3InputEvent* fxInput;

   ClassDef(TMeshB3AnlProc,1)
};

#endif //TMeshB3AnlProcESSOR_H
