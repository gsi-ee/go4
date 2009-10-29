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

#ifndef TGO4FOLDERPROXY_H
#define TGO4FOLDERPROXY_H

#include "TGo4Proxy.h"

#include "TFolder.h"
#include "TString.h"

class TGo4FolderProxy : public TGo4Proxy {
   public:
      TGo4FolderProxy();
      TGo4FolderProxy(TFolder* f, Bool_t owner = kFALSE, const char* roofolder = "");
      virtual ~TGo4FolderProxy();

      virtual Bool_t HasSublevels() const { return fFolder!=0; }

      virtual TGo4LevelIter* MakeIter()
        { return (fFolder==0) ? 0 : ProduceIter(fFolder); }

      virtual TGo4Access* MakeProxy(const char* name)
        { return ProduceProxy(fFolder, name); }

      virtual void WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs);
      virtual void ReadData(TGo4Slot* slot, TDirectory* dir);

      virtual Int_t GetObjectKind();
      virtual const char* GetContainedClassName();

      static TFolder* LocateROOTFolder(const char* rootfolder);
      static TGo4Access* ProduceProxy(TFolder* folder, const char* name);
      static TGo4LevelIter* ProduceIter(TFolder* folder);

   protected:
      TFolder* fFolder;          //!
      Bool_t   fOwner;           //!
      TString  fRootFolderName;  //!

   ClassDef(TGo4FolderProxy, 1);
};

#endif

