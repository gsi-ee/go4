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

#ifndef TGO4DIRPROXY_H
#define TGO4DIRPROXY_H

#include "TGo4Proxy.h"

class TFile;
class TDirectory;

class TGo4DirProxy : public TGo4Proxy {
   protected:
      TDirectory * fDir{nullptr};          //!
      Bool_t       fOwner{kFALSE};         //!
      Bool_t       fReadRight{kFALSE};     //!
      TGo4Slot    *fxParentSlot{nullptr};  //!

      void SetDir(TDirectory *dir, Bool_t readright, Bool_t owner);
      void ClearDir();

   public:
      TGo4DirProxy();
      TGo4DirProxy(TDirectory *dir, Bool_t readright, Bool_t owner);
      virtual ~TGo4DirProxy();

      void Initialize(TGo4Slot *slot) override { fxParentSlot = slot; }
      void Finalize(TGo4Slot *slot) override {}

      Bool_t HasSublevels() const override { return fDir; }

      TGo4LevelIter *MakeIter() override
        { return fDir ? ProduceIter(fDir, fReadRight) : nullptr; }

      std::unique_ptr<TGo4Access> ProvideAccess(const char *name) override
        { return CreateAccess(fDir, fReadRight, name, fxParentSlot); }

      void WriteData(TGo4Slot *slot, TDirectory *dir, Bool_t onlyobjs) override;
      void ReadData(TGo4Slot *slot, TDirectory *dir) override;

      Int_t GetObjectKind() const override;
      const char *GetContainedClassName() const override;
      const char *GetContainedObjectInfo() override;
      Int_t GetObjectSizeInfo() const override;

      static std::unique_ptr<TGo4Access> CreateAccess(TDirectory *dir, Bool_t readright, const char *name, TGo4Slot *browser_slot = nullptr);
      static TGo4LevelIter *ProduceIter(TDirectory *dir, Bool_t readright);

      Bool_t UpdateObjectInFile(const char *filepath, TObject *obj);

      Bool_t IsFile() const;
      const char *GetFileName() const;

   ClassDefOverride(TGo4DirProxy, 1);
};

#endif
