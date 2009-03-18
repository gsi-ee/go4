#ifndef TMESHPROVIDERPROCESSOR_H
#define TMESHPROVIDERPROCESSOR_H

#include "TGo4EventProcessor.h"

class TMeshProviderProc : public TGo4EventProcessor {

   public:
      TMeshProviderProc();
      TMeshProviderProc(const char* name);
      virtual ~TMeshProviderProc() ;

      /** This method will check if input event is correct and
      * might reset the input event pointer for the subsequent steps */
      void SetRealInput();

   ClassDef(TMeshProviderProc,1)
};
#endif //TMeshProviderProcESSOR_H
