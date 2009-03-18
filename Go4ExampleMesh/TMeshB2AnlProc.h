#ifndef TMESHB2ANLPROCESSOR_H
#define TMESHB2ANLPROCESSOR_H

#include "TGo4EventProcessor.h"

class TMeshB2InputEvent;
class TMeshB2OutputEvent;

class TMeshB2AnlProc : public TGo4EventProcessor {

   public:
      TMeshB2AnlProc() ;
      TMeshB2AnlProc(const char* name);
      void Analysis(TMeshB2OutputEvent* target);
      virtual ~TMeshB2AnlProc() ;

      TMeshB2InputEvent* fxInput;

   ClassDef(TMeshB2AnlProc,1)
};
#endif //TMeshB2AnlProcESSOR_H



