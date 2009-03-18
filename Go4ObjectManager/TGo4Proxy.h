#ifndef TGO4PROXY_H
#define TGO4PROXY_H

#include "TObject.h"

class TGo4Slot;
class TDirectory;

class TGo4ObjectReceiver {
   friend class TGo4Access;
   public:
      virtual ~TGo4ObjectReceiver() {}
   protected:
      virtual Bool_t AssignObject(const char* path, TObject* obj, Bool_t ownership) = 0;
};

//*********************************************************

class TGo4Access {
   public:
      enum { kndNone = -1,
             kndObject = 0,
             kndFolder = 1,
             kndTreeBranch = 2,
             kndTreeLeaf = 3,
             kndGo4Param = 4,
             kndDataMember = 5,
             kndEventElement = 6 };

      virtual ~TGo4Access() {}

      virtual Bool_t IsRemote() const { return kFALSE; }
      virtual Bool_t CanGetObject() const;
      virtual Bool_t GetObject(TObject* &obj, Bool_t &owner) const;
      virtual TClass* GetObjectClass() const;
      virtual const char* GetObjectName() const;
      virtual const char* GetObjectClassName() const;
      /** Have to assign object to provided receiver. Returns:
        * 0 - if object can not be assigned
        * 1 - if object assigned
        * 2 - if object will be assigned later */
      virtual Int_t AssignObjectTo(TGo4ObjectReceiver* rcv, const char* path);

    protected:
      void DoObjectAssignement(TGo4ObjectReceiver* rcv,
                               const char* path,
                               TObject* obj,
                               Bool_t owner)
      {
         rcv->AssignObject(path, obj, owner);
      }
};

// ************************************************************

class TGo4LevelIter : public TObject {
   public:
      TGo4LevelIter() : TObject() {}
      virtual ~TGo4LevelIter() {}

      virtual Bool_t next() { return kFALSE; }
      virtual Bool_t isfolder() { return kFALSE; }
      virtual Bool_t isslotsfolder() { return kFALSE; }
      virtual Int_t getflag(const char*) { return -1; }
      virtual TGo4LevelIter* subiterator() { return 0; }
      virtual TGo4Slot* getslot() { return 0; }
      virtual const char* name() { return 0; }
      virtual const char* info() { return 0; }
      virtual Int_t sizeinfo() { return 0; }

      virtual Int_t GetKind() { return -1; }
      virtual const char* GetClassName() { return 0; }
};

// *************************************************************

class TGo4Proxy : public TObject {
   public:
      TGo4Proxy() : TObject() {}
      virtual ~TGo4Proxy() {}

      virtual void Initialize(TGo4Slot* slot) {}
      virtual void Finalize(TGo4Slot* slot) {}
      virtual Bool_t RemoveRegisteredObject(TObject* obj) { return kFALSE; }

      virtual Bool_t Use() const { return kTRUE; }

      virtual Bool_t HasSublevels() const { return kFALSE; }
      virtual TGo4Access* MakeProxy(const char*) { return 0; }
      virtual TGo4LevelIter* MakeIter() { return 0; }

      virtual Int_t GetObjectKind() {  return TGo4Access::kndNone; }
      virtual const char* GetContainedClassName() { return 0; }
      virtual const char* GetContainedObjectInfo() { return 0; }
      virtual Int_t GetObjectSizeInfo() { return -1; }

      virtual void WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs) {}
      virtual void ReadData(TGo4Slot* slot, TDirectory* dir) {}

      virtual Bool_t IsAcceptObject(TClass* cl) { return kFALSE; }
      virtual Bool_t AssignObject(TGo4Slot* slot, TObject* obj, Bool_t owner) { return kFALSE; }
      virtual TObject* GetAssignedObject() { return 0; }
      virtual void Update(TGo4Slot* slot, Bool_t strong) {}

      virtual Bool_t ProcessEvent(TGo4Slot* slot, TGo4Slot* source, Int_t id, void* param) { return kTRUE; }

   protected:


   ClassDef(TGo4Proxy, 1);
};

#endif
