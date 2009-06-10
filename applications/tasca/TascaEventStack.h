//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef TascaEventStack_H
#define TascaEventStack_H

#include "TList.h"
#include "TascaEvent.h"


class TascaEventStack : public TList {
   public:
      TascaEventStack();
      TascaEventStack(UInt_t slots);
      virtual ~TascaEventStack();

      Int_t PrintEventStack();

	UInt_t entries;
	UInt_t used;
   ClassDef(TascaEventStack,1)
};

#endif
