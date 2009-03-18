#ifndef TGO4LINKPROXY_H
#define TGO4LINKPROXY_H

#include "TGo4Proxy.h"

class TString;

class TGo4LinkProxy : public TGo4Proxy {
   public:
      TGo4LinkProxy();
      TGo4LinkProxy(TGo4Slot* linkedslot);
      virtual ~TGo4LinkProxy();

      virtual void Initialize(TGo4Slot* slot);
      virtual void Finalize(TGo4Slot* slot);

      virtual TGo4Access* MakeProxy(const char* name);
      virtual TGo4LevelIter* MakeIter();

      virtual Int_t GetObjectKind();
      virtual const char* GetContainedClassName();

      virtual void WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs);
      virtual void ReadData(TGo4Slot* slot, TDirectory* dir);

      virtual Bool_t IsAcceptObject(TClass* cl);
      virtual Bool_t AssignObject(TGo4Slot* slot, TObject* obj, Bool_t owner);
      virtual TObject* GetAssignedObject();

      virtual Bool_t ProcessEvent(TGo4Slot* slot, TGo4Slot* source, Int_t id, void* param);

      TGo4Slot* GetLink() const { return fLink; }

   protected:
      TGo4Slot*     fLink;     //!

   ClassDef(TGo4LinkProxy, 1);
};

#endif
