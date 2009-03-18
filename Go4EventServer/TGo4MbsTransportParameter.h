#ifndef TGO4MBSTRANSPORTPARAMETER_H
#define TGO4MBSTRANSPORTPARAMETER_H

#include "TGo4EventSourceParameter.h"

class TGo4MbsTransportParameter : public TGo4EventSourceParameter {
  public:
    TGo4MbsTransportParameter() ;

    TGo4MbsTransportParameter(const char* name);

    virtual ~TGo4MbsTransportParameter() ;

  ClassDef(TGo4MbsTransportParameter,1)
};

#endif //TGO4MBSTRANSPORTPARAMETER_H
