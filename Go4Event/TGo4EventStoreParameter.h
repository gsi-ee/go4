#ifndef TGO4EVENTSTOREPARAMETER_H
#define TGO4EVENTSTOREPARAMETER_H

#include "TGo4Parameter.h"

/**
 * Basic type for all classes containing information
 * to parametrize the event store. For use in the
 * event factory; factory method gets (possibly subclassed)
 * parameter object which is suited to the concrete user
 * factory. Name string can be used as filename, e.g.
 */
class TGo4EventStoreParameter : public TGo4Parameter {
  public:
    TGo4EventStoreParameter() ;

    TGo4EventStoreParameter(const char* name, Int_t id = 0);

    virtual ~TGo4EventStoreParameter() ;

    Int_t GetID() const { return fiID; }

    /** basic method to printout status information
      * on stdout; to be overridden by specific subclass */
    virtual Int_t PrintParameter(Text_t* buffer=0, Int_t buflen=0);

    /** update contents of paramter class with external object. */
    virtual Bool_t UpdateFrom(TGo4Parameter* rhs);

  private:
    /** Identity number to specify the kind. Optional, default is 0. */
     Int_t fiID;

  ClassDef(TGo4EventStoreParameter,1)
};

#endif //TGO4EVENTSTOREPARAMETER_H
