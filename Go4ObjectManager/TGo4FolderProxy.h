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

#ifndef TGO4FOLDERPROXY_H
#define TGO4FOLDERPROXY_H

#include "TGo4Proxy.h"
#include "TString.h"

class TFolder;

class TGo4FolderProxy : public TGo4Proxy {
   public:
      TGo4FolderProxy();
      TGo4FolderProxy(TFolder *f, Bool_t owner = kFALSE, const char *roofolder = "");
      virtual ~TGo4FolderProxy();

      Bool_t HasSublevels() const override { return fFolder; }

      TGo4LevelIter* MakeIter() override
        { return fFolder ? ProduceIter(fFolder) : nullptr; }

      std::unique_ptr<TGo4Access> ProvideAccess(const char *name) override
        { return CreateAccess(fFolder, name); }

      void WriteData(TGo4Slot *slot, TDirectory *dir, Bool_t onlyobjs) override;
      void ReadData(TGo4Slot *slot, TDirectory *dir) override;

      Int_t GetObjectKind() const override;
      const char *GetContainedClassName() const override;

      static TFolder *LocateROOTFolder(const char *rootfolder);
      static std::unique_ptr<TGo4Access> CreateAccess(TFolder *folder, const char *name);
      static TGo4LevelIter* ProduceIter(TFolder *folder);

   protected:
      TFolder *fFolder{nullptr}; //!
      Bool_t   fOwner{kFALSE};   //!
      TString  fRootFolderName;  //!

   ClassDefOverride(TGo4FolderProxy, 1);
};

#endif

