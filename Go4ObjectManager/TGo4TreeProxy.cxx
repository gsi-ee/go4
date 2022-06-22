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

#include "TGo4TreeProxy.h"

#include "TTree.h"
#include "TBranch.h"
#include "TIterator.h"

#include "TGo4ObjectProxy.h"

class TGo4BranchAccess : public TGo4Access {
   public:
      TGo4BranchAccess(TBranch* br) : TGo4Access(), fBranch(br) {}

      const char* GetObjectName() const override
        { return fBranch->GetName(); }

      const char* GetObjectClassName() const override
        { return fBranch->ClassName(); }

   private:
      TBranch*   fBranch{nullptr};  //!
};

// ****************************************************************

class TGo4TreeLevelIter : public TGo4LevelIter {
   public:
      TGo4TreeLevelIter(TTree* tree) :
         TGo4LevelIter()
      {
         fIter = tree->GetListOfBranches()->MakeIterator();
      }

      TGo4TreeLevelIter(TBranch* branch) :
         TGo4LevelIter()
      {
         fIter = branch->GetListOfBranches()->MakeIterator();
      }

      virtual ~TGo4TreeLevelIter()
      {
         delete fIter;
      }

      Bool_t next() override
      {
         do {
            TObject* res = fIter->Next();
            if (!res) return kFALSE;
            fCurrent = dynamic_cast<TBranch*> (res);
         } while (!fCurrent);
         return kTRUE;
      }

      Bool_t isfolder() override
      {
         return fCurrent->GetListOfBranches()->GetEntries() > 0;
      }

      TGo4LevelIter* subiterator() override
      {
        return new TGo4TreeLevelIter(fCurrent);
      }

      const char* name() override
      {
         return fCurrent->GetName();
      }

      const char* info() override
      {
         return fCurrent->GetClassName();
      }

      Int_t GetKind() override
      {
         return isfolder() ? TGo4Access::kndTreeBranch : TGo4Access::kndTreeLeaf;
      }

      const char* GetClassName() override
      {
         return fCurrent->ClassName();
      }

   protected:
      TIterator*     fIter{nullptr};     //!
      TBranch*       fCurrent{nullptr};  //!
};

// ***********************************************************************

TGo4TreeProxy::TGo4TreeProxy() :
   TGo4Proxy(),
   fTree(nullptr),
   fOwner(kFALSE)
{
}

TGo4TreeProxy::TGo4TreeProxy(TTree* tree, Bool_t owner) :
   TGo4Proxy(),
   fTree(tree),
   fOwner(owner)
{
}

TGo4TreeProxy::~TGo4TreeProxy()
{
   if (fOwner) delete fTree;
}

Int_t TGo4TreeProxy::GetObjectKind()
{
   return fTree ? TGo4Access::kndFolder : TGo4Access::kndNone;
}


const char* TGo4TreeProxy::GetContainedClassName()
{
   return fTree ? fTree->ClassName() : nullptr;
}

TGo4Access* TGo4TreeProxy::CreateAccess(TTree* tree, const char* name)
{
   if (!tree) return nullptr;

   if (!name || (*name == 0)) return new TGo4ObjectAccess(tree);

   TObjArray* list = tree->GetListOfBranches();
   const char* curname = name;

   while (list) {
      const char* slash = strchr(curname,'/');
      UInt_t len = slash ? slash - curname : strlen(curname);
      TIter iter(list);
      TObject* obj = nullptr;
      while ((obj = iter()) != nullptr)
         if ((strlen(obj->GetName()) == len) &&
             (strncmp(obj->GetName(), curname, len) == 0)) break;
      TBranch* br = dynamic_cast<TBranch*> (obj);
      if (!br) return nullptr;

      if (slash) {
         list = br->GetListOfBranches();
         curname = slash+1;
      } else
         return new TGo4BranchAccess(br);
   }
   return nullptr;
}

TGo4LevelIter* TGo4TreeProxy::ProduceIter(TTree* tree)
{
   return new TGo4TreeLevelIter(tree);
}
