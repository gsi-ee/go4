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

#include "TGo4FolderProxy.h"

#include "TROOT.h"
#include "TTree.h"
#include "TFolder.h"
#include "TCanvas.h"
#include "THStack.h"

#include "TGo4ObjectProxy.h"
#include "TGo4DirProxy.h"
#include "TGo4TreeProxy.h"
#include "TGo4CanvasProxy.h"
#include "TGo4HStackProxy.h"
#include "TGo4Slot.h"


class TGo4FolderLevelIter : public TGo4LevelIter {
   public:
      TGo4FolderLevelIter(TFolder* folder) :
         TGo4LevelIter()
      {
         fIter = folder->GetListOfFolders()->MakeIterator();
      }

      virtual ~TGo4FolderLevelIter()
      {
         delete fIter;
      }

      Bool_t next() override
      {
         fCurrent = fIter->Next();
         return fCurrent;
      }

      Bool_t isfolder() override
      {
         return (dynamic_cast<TFolder*>(fCurrent) != nullptr) ||
                (dynamic_cast<TDirectory*>(fCurrent) != nullptr) ||
                (dynamic_cast<TTree*>(fCurrent) != nullptr) ||
                (dynamic_cast<TCanvas*>(fCurrent) != nullptr) ||
                (dynamic_cast<THStack*>(fCurrent) != nullptr);
      }

      TGo4LevelIter* subiterator() override
      {
         TTree* tr = dynamic_cast<TTree*>(fCurrent);
         if (tr) return TGo4TreeProxy::ProduceIter(tr);
         TDirectory* dir = dynamic_cast<TDirectory*>(fCurrent);
         if (dir) return TGo4DirProxy::ProduceIter(dir, kFALSE);
         TCanvas* canv = dynamic_cast<TCanvas*>(fCurrent);
         if (canv) return TGo4CanvasProxy::ProduceIter(canv);
         THStack* hs = dynamic_cast<THStack*> (fCurrent);
         if (hs)  return TGo4HStackProxy::ProduceIter(hs);
         return new TGo4FolderLevelIter((TFolder*)fCurrent);
      }

      const char *name() override
      {
         return fCurrent->GetName();
      }

      const char* info() override
      {
         return fCurrent->ClassName();
      }

      Int_t GetKind() override
      {
         return isfolder() ? TGo4Access::kndFolder : TGo4Access::kndObject;
      }

      const char* GetClassName() override
      {
         return fCurrent->ClassName();
      }

   protected:
      TIterator*     fIter{nullptr};     //!
      TObject*       fCurrent{nullptr};  //!
};

// ****************************************************************

TGo4FolderProxy::TGo4FolderProxy() :
   TGo4Proxy()
{
}

TGo4FolderProxy::TGo4FolderProxy(TFolder* f, Bool_t owner, const char* rootfolder) :
   TGo4Proxy(),
   fFolder(f),
   fOwner(owner),
   fRootFolderName(rootfolder)
{
}

TGo4FolderProxy::~TGo4FolderProxy()
{
   if (fOwner) delete fFolder;
}

Int_t TGo4FolderProxy::GetObjectKind()
{
   return fFolder ? TGo4Access::kndFolder : TGo4Access::kndNone;
}

const char* TGo4FolderProxy::GetContainedClassName()
{
   return fFolder ? fFolder->ClassName() : nullptr;
}

void TGo4FolderProxy::WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs)
{
   if (!onlyobjs) {
      const char* foldername = fRootFolderName.Length() > 0 ? fRootFolderName.Data() : nullptr;
      slot->SetPar("FolderProxy::RootFolder", foldername);
   }
}

void TGo4FolderProxy::ReadData(TGo4Slot* slot, TDirectory* dir)
{
   const char* foldername = slot->GetPar("FolderProxy::RootFolder");

   fFolder = LocateROOTFolder(foldername);
   if (fFolder)
      fRootFolderName = foldername;
   fOwner = kFALSE;
}

TFolder* TGo4FolderProxy::LocateROOTFolder(const char* rootfolder)
{
   TFolder* res = nullptr;
   if (rootfolder) {
     if (strcmp(rootfolder,"//root/") == 0)
        res = gROOT->GetRootFolder();
     else
       res = dynamic_cast<TFolder*> (gROOT->GetRootFolder()->FindObject(rootfolder));
   }
   return res;
}


TGo4Access* TGo4FolderProxy::CreateAccess(TFolder* folder, const char *name)
{
   if (!folder) return nullptr;
   if (!name || (*name == 0)) return new TGo4ObjectAccess(folder);

   TFolder* curfold = folder;
   const char* curname = name;

   while (curfold) {
      const char* slash = strchr(curname,'/');
      UInt_t len = slash ? slash - curname : strlen(curname);
      TIter iter(curfold->GetListOfFolders());
      TObject *obj = nullptr;
      while ((obj = iter()) != nullptr)
         if ((strlen(obj->GetName()) == len) &&
             (strncmp(obj->GetName(), curname, len) == 0)) break;
      if (!obj) return nullptr;

      if (!slash)
         return new TGo4ObjectAccess(obj);

      curname = slash+1;

      TTree* tr = dynamic_cast<TTree*> (obj);
      if (tr)
         return TGo4TreeProxy::CreateAccess(tr, curname);

      TDirectory* dir = dynamic_cast<TDirectory*> (obj);
      if (dir)
         return TGo4DirProxy::CreateAccess(dir, kFALSE, curname);

      TCanvas* canv = dynamic_cast<TCanvas*> (obj);
      if (canv)
         return TGo4CanvasProxy::CreateAccess(canv, curname);

      THStack* hs = dynamic_cast<THStack*> (obj);
      if (hs)
         return TGo4HStackProxy::CreateAccess(hs, curname);

      curfold = dynamic_cast<TFolder*>(obj);
   }

   return nullptr;
}

TGo4LevelIter* TGo4FolderProxy::ProduceIter(TFolder* folder)
{
   return new TGo4FolderLevelIter(folder);
}
