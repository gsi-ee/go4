#ifndef TXXXANLFACTORY_H
#define TXXXANLFACTORY_H

#include "TGo4EventServerFactory.h"

class TXXXAnlFact : public TGo4EventServerFactory {
   public:
      TXXXAnlFact() ;
      TXXXAnlFact(const char* name);
      virtual ~TXXXAnlFact() ;

      virtual TGo4EventElement   * CreateInputEvent();
      virtual TGo4EventElement   * CreateOutputEvent();
      virtual TGo4EventProcessor * CreateEventProcessor(TGo4EventProcessorParameter* par);

   ClassDef(TXXXAnlFact,1)
};
#endif //TXXXANLFACTORY_H



