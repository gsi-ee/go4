//---------------------------------------------
// Go4 Comp analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef CompEventStack_H
#define CompEventStack_H

#include "TList.h"
#include "CompEvent.h"


class CompEventStack : public TList {
   public:
      CompEventStack();
      CompEventStack(UInt_t slots);
      virtual ~CompEventStack();

      Int_t PrintEventStack();

	UInt_t entries;
	UInt_t used;
   ClassDef(CompEventStack,1)
};

#endif
