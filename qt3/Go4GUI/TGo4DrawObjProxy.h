#ifndef TGO4DRAWOBJPROXY_H
#define TGO4DRAWOBJPROXY_H

#include "TGo4ObjectProxy.h"

class TGo4DrawObjProxy : public TGo4ObjectProxy {
   public:
      TGo4DrawObjProxy(TObject* obj, Bool_t owner, Bool_t doclear = kFALSE);
      virtual ~TGo4DrawObjProxy();
      virtual Bool_t RemoveRegisteredObject(TObject* obj);

   protected:
      void TryToClearObject();

      Bool_t  fDoClear;   //!
};

#endif
