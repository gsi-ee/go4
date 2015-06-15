// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
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
      virtual TGo4Access* ProvideAccess(const char*) { return 0; }
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

      static TClass* GetClass(const char* classname, Bool_t load = kFALSE);

   ClassDef(TGo4Proxy, 1);
};


#endif
