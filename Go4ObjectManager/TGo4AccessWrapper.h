#ifndef TGO4ACCESSWRAPPER_H
#define TGO4ACCESSWRAPPER_H

#include "TGo4Proxy.h"

class TGo4AccessWrapper {
   public:
      TGo4AccessWrapper() {}
      virtual ~TGo4AccessWrapper() {}

      TObject* GetObject(const char* name = 0)
      {
        TGo4Access* proxy = ProvideAccess(name);
        TObject* res = 0;
        Bool_t owner = kFALSE;
        if (proxy) proxy->GetObject(res, owner);
        delete proxy;
        return res;
      }

      TClass* GetObjectClass(const char* name = 0)
      {
         TGo4Access* proxy = ProvideAccess(name);
         TClass* res = proxy ? proxy->GetObjectClass() : 0;
         delete proxy;
         return res;
      }

      const char* GetObjectClassName(const char* name = 0)
      {
         TGo4Access* proxy = ProvideAccess(name);
         const char* res = proxy ? proxy->GetObjectClassName() : 0;
         delete proxy;
         return res;
      }

      virtual TGo4Access* ProvideAccess(const char* name = 0) { return 0; }

   protected:


   ClassDef(TGo4AccessWrapper,1);
};

#endif
