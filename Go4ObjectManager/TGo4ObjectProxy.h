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

#ifndef TGO4OBJECTPROXY_H
#define TGO4OBJECTPROXY_H

#include "TGo4Proxy.h"

class TGo4ObjectAccess : public TGo4Access {
   public:
      TGo4ObjectAccess(TObject *obj);
      Bool_t CanGetObject() const override;
      Bool_t GetObject(TObject *&obj, Bool_t &owner) const override;
      TClass *GetObjectClass() const override;
      const char *GetObjectName() const override;
      const char *GetObjectClassName() const override;

   private:
      TObject   *fObject{nullptr};    //!
};

// **************************************************************

class TGo4ObjectProxy : public TGo4Proxy {
   public:
      TGo4ObjectProxy();
      TGo4ObjectProxy(TObject *obj, Bool_t owner = kFALSE);
      virtual ~TGo4ObjectProxy();

      void Initialize(TGo4Slot *slot) override;
      void Finalize(TGo4Slot *slot) override;
      Bool_t RemoveRegisteredObject(TObject *obj) override;

      std::unique_ptr<TGo4Access> ProvideAccess(const char *name) override;
      TGo4LevelIter *MakeIter() override { return nullptr; }

      Int_t GetObjectKind() const override;
      const char *GetContainedClassName() const override;
      const char *GetContainedObjectInfo() override;
      Int_t GetObjectSizeInfo() const override;

      void WriteData(TGo4Slot *slot, TDirectory *dir, Bool_t onlyobjs) override;
      void ReadData(TGo4Slot *slot, TDirectory *dir) override;

      Bool_t IsAcceptObject(TClass *cl) const override;
      Bool_t AssignObject(TGo4Slot *slot, TObject *obj, Bool_t owner) override;
      TObject *GetAssignedObject() override;

      static Long_t DefineObjectSize(TObject *obj);

   protected:

      TObject      *fObject{nullptr}; //!
      Bool_t        fOwner{kFALSE};   //!

   ClassDefOverride(TGo4ObjectProxy, 1);
};

#endif
