//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef TascaEventStack_H
#define TascaEventStack_H

#include "TList.h"


class TascaEventStack : public TList {
   public:
      TascaEventStack();
      TascaEventStack(const char* name);
      virtual ~TascaEventStack();

      Int_t PrintEventStack();


   ClassDef(TascaEventStack,1)
};

#endif
