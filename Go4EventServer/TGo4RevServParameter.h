#ifndef TGO4REVSERVPARAMETER_H
#define TGO4REVSERVPARAMETER_H

#include "TGo4EventSourceParameter.h"

class TGo4RevServParameter : public TGo4EventSourceParameter {
  public:
    TGo4RevServParameter();

    TGo4RevServParameter(const char* name);

    virtual ~TGo4RevServParameter() ;

    void SetPort(Int_t port) { fiPort=port; }

    Int_t GetPort() const { return fiPort; }

    virtual Int_t PrintParameter(Text_t* buffer=0, Int_t buflen=0);

  private:

    /** Port number .  */
    Int_t fiPort;

  ClassDef(TGo4RevServParameter,2)
};

#endif //TGO4REVSERVPARAMETER_H
