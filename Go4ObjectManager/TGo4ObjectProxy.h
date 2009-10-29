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

#ifndef TGO4OBJECTPROXY_H
#define TGO4OBJECTPROXY_H

#include "TGo4Proxy.h"

class TGo4ObjectAccess : public TGo4Access {
   public:
      TGo4ObjectAccess(TObject* obj);
      virtual Bool_t CanGetObject() const;
      virtual Bool_t GetObject(TObject* &obj, Bool_t &owner) const;
      virtual TClass* GetObjectClass() const;
      virtual const char* GetObjectName() const;
      virtual const char* GetObjectClassName() const;

   private:
      TObject*  fObject;    //!
};

// **************************************************************

class TGo4ObjectProxy : public TGo4Proxy {
   public:
      TGo4ObjectProxy();
      TGo4ObjectProxy(TObject* obj, Bool_t owner = kFALSE);
      virtual ~TGo4ObjectProxy();

      virtual void Initialize(TGo4Slot* slot);
      virtual void Finalize(TGo4Slot* slot);
      virtual Bool_t RemoveRegisteredObject(TObject* obj);

      virtual TGo4Access* MakeProxy(const char* name);
      virtual TGo4LevelIter* MakeIter() { return 0; }

      virtual Int_t GetObjectKind();
      virtual const char* GetContainedClassName();
      virtual const char* GetContainedObjectInfo();
      virtual Int_t GetObjectSizeInfo();

      virtual void WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs);
      virtual void ReadData(TGo4Slot* slot, TDirectory* dir);

      virtual Bool_t IsAcceptObject(TClass* cl);
      virtual Bool_t AssignObject(TGo4Slot* slot, TObject* obj, Bool_t owner);
      virtual TObject* GetAssignedObject();

   protected:

      TObject*      fObject;   //!
      Bool_t        fOwner;   //!

   ClassDef(TGo4ObjectProxy, 1);
};

#endif
