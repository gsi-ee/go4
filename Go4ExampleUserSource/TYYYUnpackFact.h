#ifndef TYYYUNPACKFACTORY_H
#define TYYYUNPACKFACTORY_H

#include "TGo4EventServerFactory.h"

class TYYYUnpackFact : public TGo4EventServerFactory {
   public:
      TYYYUnpackFact() ;
      TYYYUnpackFact(const char* name);
      virtual ~TYYYUnpackFact() ;

      virtual TGo4EventElement   * CreateInputEvent();
      virtual TGo4EventElement   * CreateOutputEvent();
      virtual TGo4EventProcessor * CreateEventProcessor(TGo4EventProcessorParameter* par);
      virtual TGo4EventSource* CreateEventSource(TGo4EventSourceParameter* par);

   ClassDef(TYYYUnpackFact,1)
};
#endif //TYYYUNPACKFACTORY_H



