#ifndef TGO4EVENTSOURCEPARAMETER_H
#define TGO4EVENTSOURCEPARAMETER_H

#include "TGo4Parameter.h"

/**
 * Basic type for all classes containing information
 * to parametrize the event source. For use in the
 * event factory; factory method gets (possibly subclassed)
 * parameter object which is suited to the concrete user
 * factory. Name string can be used as filename, e.g.
 */
class TGo4EventSourceParameter : public TGo4Parameter {
   public:
      TGo4EventSourceParameter() ;

      TGo4EventSourceParameter(const char* name, Int_t id = 0);

      virtual ~TGo4EventSourceParameter() ;

      Int_t GetID() const { return fiID; }

      Int_t GetTimeout() const { return fiTimeout; }

      void SetTimeout(Int_t out) { fiTimeout=out; }

      /** basic method to printout status information
        * on stdout; to be overridden by specific subclass */
      virtual Int_t PrintParameter(Text_t* buffer=0, Int_t buflen=0);

      /** update contents of paramter class with external object. */
      virtual Bool_t UpdateFrom(TGo4Parameter* rhs);

   private:

      /** Identity number to specify the kind. Optional, default is 0. */
      Int_t fiID;

      /** Timeout in seconds. If -1, no timeout (default)  */
      Int_t fiTimeout;

   ClassDef(TGo4EventSourceParameter,1)
};

#endif //TGO4EVENTSOURCEPARAMETER_H
