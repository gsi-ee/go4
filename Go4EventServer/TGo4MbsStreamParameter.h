#ifndef TGO4MBSSTREAMPARAMETER_H
#define TGO4MBSSTREAMPARAMETER_H

#include "TGo4EventSourceParameter.h"

class TGo4MbsStreamParameter : public TGo4EventSourceParameter {
  public:
    TGo4MbsStreamParameter() ;

    TGo4MbsStreamParameter(const char* name);

    virtual ~TGo4MbsStreamParameter() ;

  ClassDef(TGo4MbsStreamParameter,1)
};

#endif //TGO4MBSSTREAMPARAMETER_H
