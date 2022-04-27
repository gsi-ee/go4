// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4PROXY_H
#define TGO4PROXY_H

#include "TObject.h"

class TGo4Slot;
class TDirectory;
class TGo4ObjectManager;

//*********************************************************

class TGo4Access {
   public:
      enum {
         kndNone = -1,
         kndObject = 0,           // normal object
         kndFolder = 1,           // folder with sub-items
         kndTreeBranch = 2,       // tree brunch
         kndTreeLeaf = 3,         // tree leaf
         kndGo4Param = 4,         // go4 parameter
         kndDataMember = 5,       // object data member
         kndEventElement = 6,     // event object
         kndMoreFolder = 7,       // remote folder with potentially more items
         kndRootCommand = 8       // command registered in ROOT http server
      };

      virtual ~TGo4Access() {}

      /** Indicate if object is remote */
      virtual Bool_t IsRemote() const { return kFALSE; }

      /** Returns kTRUE, when object can be directly obtained with GetObject() method */
      virtual Bool_t CanGetObject() const { return kFALSE; }

      virtual Bool_t GetObject(TObject* &obj, Bool_t &owner) const;
      virtual TClass* GetObjectClass() const;
      virtual const char* GetObjectName() const;
      virtual const char* GetObjectClassName() const;

      /** Have to assign object to provided receiver. Returns:
        * 0 - if object can not be assigned
        * 1 - if object assigned
        * 2 - if object will be assigned later */
      virtual Int_t AssignObjectTo(TGo4ObjectManager* rcv, const char* path);

      /** Made for convenience - extracts OM and slot path and
       *  calls AssignObjectTo method */
      Int_t AssignObjectToSlot(TGo4Slot* slot);

    protected:
      void DoObjectAssignement(TGo4ObjectManager* rcv,
                               const char* path,
                               TObject* obj,
                               Bool_t owner);
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
      virtual TGo4LevelIter* subiterator() { return nullptr; }
      virtual TGo4Slot* getslot() { return nullptr; }
      virtual const char* name() { return nullptr; }
      virtual const char* info() { return nullptr; }
      virtual Int_t sizeinfo() { return 0; }

      virtual Int_t GetKind() { return -1; }
      virtual const char* GetClassName() { return nullptr; }
};

// *************************************************************

class TGo4Proxy : public TObject {
   public:
      TGo4Proxy() : TObject() {}
      virtual ~TGo4Proxy() {}

      virtual void Initialize(TGo4Slot*) {}
      virtual void Finalize(TGo4Slot*) {}
      virtual Bool_t RemoveRegisteredObject(TObject*) { return kFALSE; }

      virtual Bool_t Use() const { return kTRUE; }

      virtual Bool_t HasSublevels() const { return kFALSE; }
      virtual TGo4Access* ProvideAccess(const char*) { return nullptr; }
      virtual TGo4LevelIter* MakeIter() { return nullptr; }

      virtual Int_t GetObjectKind() {  return TGo4Access::kndNone; }
      virtual const char* GetContainedClassName() { return nullptr; }
      virtual const char* GetContainedObjectInfo() { return nullptr; }
      virtual Int_t GetObjectSizeInfo() { return -1; }

      virtual void WriteData(TGo4Slot*, TDirectory*, Bool_t) {}
      virtual void ReadData(TGo4Slot*, TDirectory*) {}

      virtual Bool_t IsAcceptObject(TClass*) { return kFALSE; }
      virtual Bool_t AssignObject(TGo4Slot*, TObject*, Bool_t) { return kFALSE; }
      virtual TObject* GetAssignedObject() { return nullptr; }
      virtual void Update(TGo4Slot*, Bool_t) {}

      virtual Bool_t ProcessEvent(TGo4Slot* slot, TGo4Slot* source, Int_t id, void* param) { return kTRUE; }

      static TClass* GetClass(const char* classname, Bool_t load = kFALSE);

   ClassDef(TGo4Proxy, 1);
};

#endif
