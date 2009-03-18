#ifndef TGO4SIMPLEEVENTPROCESSOR_H
#define TGO4SIMPLEEVENTPROCESSOR_H

#include "TGo4EventProcessor.h"

class TGo4SimpleEvent;

class TGo4SimpleEventProcessor : public TGo4EventProcessor {
   public:

      TGo4SimpleEventProcessor();

      void BuildSimpleEvent(TGo4SimpleEvent* target);

      virtual ~TGo4SimpleEventProcessor() ;

  ClassDef(TGo4SimpleEventProcessor,1)
};

#endif //TGO4SIMPLEEVENTPROCESSOR_H
