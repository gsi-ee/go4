#ifndef SCONTROL_H
#define SCONTROL_H

#include "TGo4Parameter.h"

class TXXXControl : public TGo4Parameter {
   public:
     TXXXControl();
     TXXXControl(const char* name);
     virtual ~TXXXControl();
     virtual Int_t    PrintParameter(Text_t * n, Int_t);
     virtual Bool_t   UpdateFrom(TGo4Parameter *);
     void     SaveMacro();

     // control filling of histograms

     Bool_t   fill;
   ClassDef(TXXXControl,1)
};

#endif //SCONTROL_H
