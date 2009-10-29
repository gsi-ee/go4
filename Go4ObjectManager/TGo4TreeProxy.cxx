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

#include "TGo4TreeProxy.h"

#include "TTree.h"
#include "TBranch.h"
#include "TIterator.h"
#include "TObjArray.h"

#include "TGo4ObjectProxy.h"

class TGo4BranchAccess : public TGo4Access {
   public:
      TGo4BranchAccess(TBranch* br) : TGo4Access(), fBranch(br) {}

      virtual const char* GetObjectName() const
        { return fBranch->GetName(); }

      virtual const char* GetObjectClassName() const
        { return fBranch->GetClassName(); }

   private:
      TBranch*   fBranch;  //!
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

      virtual Bool_t next()
      {
         do {
            TObject* res = fIter->Next();
            if (res==0) return kFALSE;
            fCurrent = dynamic_cast<TBranch*> (res);
         } while (fCurrent==0);
         return kTRUE;
      }

      virtual Bool_t isfolder()
      {
         return fCurrent->GetListOfBranches()->GetEntries() > 0;
      }

      virtual TGo4LevelIter* subiterator()
      {
        return new TGo4TreeLevelIter(fCurrent);
      }

      virtual const char* name()
      {
         return fCurrent->GetName();
      }

      virtual const char* info()
      {
         return fCurrent->GetClassName();
      }

      virtual Int_t GetKind()
      {
         return isfolder() ? TGo4Access::kndTreeBranch : TGo4Access::kndTreeLeaf;
      }

      virtual const char* GetClassName()
      {
         return fCurrent->GetClassName();
      }
   protected:
      TIterator*     fIter;     //!
      TBranch*       fCurrent;  //!
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

TGo4Access* TGo4TreeProxy::ProduceProxy(TTree* tree, const char* name)
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
