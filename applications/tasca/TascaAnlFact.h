#ifndef TascaANLFACTORY_H
#define TascaANLFACTORY_H

#include "TGo4EventServerFactory.h"

class TascaAnlFact : public TGo4EventServerFactory {
   public:
      TascaAnlFact() ;
      TascaAnlFact(const char* name);
      virtual ~TascaAnlFact() ;

      virtual TGo4EventElement   * CreateInputEvent();
      virtual TGo4EventElement   * CreateOutputEvent();
      virtual TGo4EventProcessor * CreateEventProcessor(TGo4EventProcessorParameter* par);

   ClassDef(TascaAnlFact,1)
};
#endif //TascaANLFACTORY_H



