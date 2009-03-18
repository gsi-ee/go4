#ifndef TGO4USERSTOREPARAMETER_H
#define TGO4USERSTOREPARAMETER_H

#include "TGo4EventStoreParameter.h"

#include "TString.h"


class TGo4EventElement;

class TGo4UserStoreParameter : public TGo4EventStoreParameter {

  public:
    TGo4UserStoreParameter();

    TGo4UserStoreParameter(const char* name);

    virtual ~TGo4UserStoreParameter() ;

    /** basic method to printout status information
      * on stdout; to be overridden by specific  subclass  */
    virtual Int_t PrintParameter(Text_t* buffer=0, Int_t buflen=0);

    /** update contents of paramter class with external object. */
    virtual Bool_t UpdateFrom(TGo4Parameter* rhs);

    /** User expression as string */
    const char* GetExpression() const { return fxExpression.Data(); }
    void SetExpression(const char* name) { fxExpression=name; }

  private:

    /** Any user defined expression as string */
    TString fxExpression;

  ClassDef(TGo4UserStoreParameter,1)
};

#endif //TGO4USERSTOREPARAMETER_H
