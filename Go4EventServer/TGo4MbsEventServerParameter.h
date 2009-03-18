#ifndef TGO4MBSEVENTSERVERPARAMETER_H
#define TGO4MBSEVENTSERVERPARAMETER_H

#include "TGo4EventSourceParameter.h"

class TGo4MbsEventServerParameter : public TGo4EventSourceParameter {
  friend class TGo4MbsEventServer;
  public:
    TGo4MbsEventServerParameter();

    TGo4MbsEventServerParameter(const char* name);

    virtual ~TGo4MbsEventServerParameter() ;

  ClassDef(TGo4MbsEventServerParameter,1)
};

#endif //TGO4MBSEVENTSERVERPARAMETER_H
