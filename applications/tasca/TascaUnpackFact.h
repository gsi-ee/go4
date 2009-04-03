#ifndef TascaUNPACKFACTORY_H
#define TascaUNPACKFACTORY_H

#include "TGo4EventServerFactory.h"

class TascaUnpackFact : public TGo4EventServerFactory {
   public:
      TascaUnpackFact();
      TascaUnpackFact(const char* name);
      virtual ~TascaUnpackFact() ;

      virtual TGo4EventElement   * CreateOutputEvent();
      virtual TGo4EventProcessor * CreateEventProcessor(TGo4EventProcessorParameter* par);

   ClassDef(TascaUnpackFact,1)
};
#endif //TascaUNPACKFACTORY_H



