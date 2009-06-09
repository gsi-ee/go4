#ifndef TGO4REVSERVPARAMETER_H
#define TGO4REVSERVPARAMETER_H

#include "TGo4MbsSourceParameter.h"

class TGo4RevServParameter : public TGo4MbsSourceParameter {
  public:
    TGo4RevServParameter();

    TGo4RevServParameter(const char* name, Int_t defport = 0);

    virtual ~TGo4RevServParameter() ;

    void SetPort(Int_t port) { fiPort=port; }

    Int_t GetPort() const { return fiPort; }

    virtual Int_t PrintParameter(Text_t* buffer=0, Int_t buflen=0);

    virtual Bool_t UpdateFrom(TGo4Parameter* rhs);

  private:

    /** Port number .  */
    Int_t fiPort;

  ClassDef(TGo4RevServParameter, 3)
};

#endif //TGO4REVSERVPARAMETER_H
