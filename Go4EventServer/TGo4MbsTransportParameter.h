#ifndef TGO4MBSTRANSPORTPARAMETER_H
#define TGO4MBSTRANSPORTPARAMETER_H

#include "TGo4MbsSourceParameter.h"

class TGo4MbsTransportParameter : public TGo4MbsSourceParameter {
  public:
    TGo4MbsTransportParameter() ;

    TGo4MbsTransportParameter(const char* name);

    virtual ~TGo4MbsTransportParameter() ;

  ClassDef(TGo4MbsTransportParameter, 2)
};

#endif //TGO4MBSTRANSPORTPARAMETER_H
