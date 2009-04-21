#ifndef TGO4MBSEVENTSERVERPARAMETER_H
#define TGO4MBSEVENTSERVERPARAMETER_H

#include "TGo4MbsSourceParameter.h"

class TGo4MbsEventServerParameter : public TGo4MbsSourceParameter {
  friend class TGo4MbsEventServer;
  public:
    TGo4MbsEventServerParameter();

    TGo4MbsEventServerParameter(const char* name);

    virtual ~TGo4MbsEventServerParameter() ;

  ClassDef(TGo4MbsEventServerParameter, 2)
};

#endif //TGO4MBSEVENTSERVERPARAMETER_H
