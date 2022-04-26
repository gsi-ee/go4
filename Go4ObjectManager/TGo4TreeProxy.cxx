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
         TGo4LevelIter(),
         fIter(0),
         fCurrent(0)
      {
         fIter = tree->GetListOfBranches()->MakeIterator();
      }

      TGo4TreeLevelIter(TBranch* branch) :
         TGo4LevelIter(),
         fIter(0),
         fCurrent(0)
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
   fTree(0),
   fOwner(0)
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
   return (fTree!=0) ? TGo4Access::kndFolder : TGo4Access::kndNone;
}


const char* TGo4TreeProxy::GetContainedClassName()
{
   return (fTree!=0) ? fTree->ClassName() : 0;
}

TGo4Access* TGo4TreeProxy::CreateAccess(TTree* tree, const char* name)
{
   if (tree==0) return 0;

   if ((name==0) || (*name==0)) return new TGo4ObjectAccess(tree);

   TObjArray* list = tree->GetListOfBranches();
   const char* curname = name;

   while (list!=0) {
      const char* slash = strchr(curname,'/');
      UInt_t len = (slash!=0) ? slash - curname : strlen(curname);
      TIter iter(list);
      TObject* obj = 0;
      while ((obj = iter())!=0)
         if ((strlen(obj->GetName())==len) &&
             (strncmp(obj->GetName(), curname, len)==0)) break;
      TBranch* br = dynamic_cast<TBranch*> (obj);
      if (br==0) return 0;

      if (slash!=0) {
         list = br->GetListOfBranches();
         curname = slash+1;
      } else
         return new TGo4BranchAccess(br);
   }
   return 0;
}

TGo4LevelIter* TGo4TreeProxy::ProduceIter(TTree* tree)
{
   return new TGo4TreeLevelIter(tree);
}
