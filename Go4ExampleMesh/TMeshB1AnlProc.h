#ifndef TMESHB1ANLPROCESSOR_H
#define TMESHB1ANLPROCESSOR_H

#include "TGo4EventProcessor.h"

class TMeshB1InputEvent;
class TMeshB1OutputEvent;

class TMeshB1AnlProc : public TGo4EventProcessor {

   public:
      TMeshB1AnlProc() ;
      TMeshB1AnlProc(const char* name);
      void Analysis(TMeshB1OutputEvent* target);
      virtual ~TMeshB1AnlProc() ;

      TMeshB1InputEvent* fxInput;

   ClassDef(TMeshB1AnlProc,1)
};

#endif //TMeshB1AnlProcESSOR_H
