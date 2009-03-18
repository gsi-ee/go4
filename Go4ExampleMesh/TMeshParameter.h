#ifndef MESHPAR_H
#define MESHPAR_H

#include "TGo4Parameter.h"

/** This parameter may be used for a global setup of the mesh */

class TMeshParameter : public TGo4Parameter {
   public:
      TMeshParameter();
      TMeshParameter(const char* name);
      virtual ~TMeshParameter();
      virtual Int_t PrintParameter(Text_t * n, Int_t);
      virtual Bool_t UpdateFrom(TGo4Parameter *);

      Bool_t fbUnpackOn; // switch on unpack step
      Bool_t fbExec1On;  // switch on branch 1 step
      Bool_t fbExec2On;  // switch on branch 2 step
      Bool_t fbExec3On;  // switch on branch 3 step
      Bool_t fbExec12On; // switch on branch 1+2 combination step
      Bool_t fbCollectorOn; // switch on final step

   ClassDef(TMeshParameter,1)
};

#endif //MESHPAR_H





