//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef TascaEvent_H
#define TascaEvent_H
#include "TascaCheckEvent.h"


class TascaEvent : public TObject {
   public:
      TascaEvent();
      TascaEvent(const char* name);
      virtual ~TascaEvent();

      Int_t PrintEvent();
      Bool_t UpdateFrom(TascaCheckEvent *);


   ClassDef(TascaEvent,1)
};

#endif
