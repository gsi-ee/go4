#ifndef TGO4MBSRANDOMPARAMETER_H
#define TGO4MBSRANDOMPARAMETER_H

#include "TGo4EventSourceParameter.h"

class TGo4MbsRandomParameter : public TGo4EventSourceParameter {

  public:

    TGo4MbsRandomParameter() ;

    TGo4MbsRandomParameter(const char* name);

    virtual ~TGo4MbsRandomParameter() ;

  ClassDef(TGo4MbsRandomParameter,1)
};

#endif //TGO4MBSRANDOMPARAMETER_H
