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

#ifndef TGO4DABCPROXY_H
#define TGO4DABCPROXY_H

#include "TGo4ServerProxy.h"
#include "TString.h"

class TH1;

class TGo4DabcProxy : public TGo4ServerProxy {
   protected:
      TString    fNodeName;
      void      *fxHierarchy{nullptr};    //!  pointer on dabc::Hierarchy class
      TGo4Slot  *fxParentSlot{nullptr};

   public:
      TGo4DabcProxy();
      virtual ~TGo4DabcProxy();

      Bool_t Connect(const char *nodename);
      Bool_t UpdateHierarchy(Bool_t sync = kTRUE);
      Bool_t ReplyCommand(void *cmd);

      const char *GetServerName() const override { return fNodeName.Data(); }

      void Initialize(TGo4Slot *slot) override;
      void Finalize(TGo4Slot *slot) override;

      Bool_t HasSublevels() const override;
      std::unique_ptr<TGo4Access> ProvideAccess(const char *name) override;
      TGo4LevelIter *MakeIter() override;

      Int_t GetObjectKind() const override {  return TGo4Access::kndFolder; }
      const char *GetContainedClassName() const override { return "TGo4DabcProxy"; }
      const char *GetContainedObjectInfo() override { return nullptr; }
      Int_t GetObjectSizeInfo() const override { return -1; }

      void WriteData(TGo4Slot *slot, TDirectory *dir, Bool_t onlyobjs) override;
      void ReadData(TGo4Slot *slot, TDirectory *dir) override;

      void Update(TGo4Slot *slot, Bool_t strong) override;

      Bool_t RefreshNamesList() override;

      static const char *GetDabcVersion();

   ClassDefOverride(TGo4DabcProxy, 1);
};

#endif
