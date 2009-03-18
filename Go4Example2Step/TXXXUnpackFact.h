#ifndef TXXXUNPACKFACTORY_H
#define TXXXUNPACKFACTORY_H

#include "TGo4EventServerFactory.h"

class TXXXUnpackFact : public TGo4EventServerFactory {
   public:
      TXXXUnpackFact();
      TXXXUnpackFact(const char* name);
      virtual ~TXXXUnpackFact() ;

      virtual TGo4EventElement   * CreateOutputEvent();
      virtual TGo4EventProcessor * CreateEventProcessor(TGo4EventProcessorParameter* par);

   ClassDef(TXXXUnpackFact,1)
};
#endif //TXXXUNPACKFACTORY_H



