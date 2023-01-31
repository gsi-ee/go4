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

#include "TGo4HStackProxy.h"

#include "TROOT.h"
#include "TList.h"
#include "TClass.h"
#include "THStack.h"

#include "TGo4ObjectProxy.h"
#include "TGo4ObjectManager.h"

class TGo4HStackLevelIter : public TGo4LevelIter {
   protected:
      TIterator*     fIter{nullptr};     //!
      TObject *      fCurrent{nullptr};  //!
   public:
      TGo4HStackLevelIter(THStack *hs) :
         TGo4LevelIter(),
         fIter(nullptr),
         fCurrent(nullptr)
      {
         fIter = hs->GetHists()->MakeIterator();
      }

      virtual ~TGo4HStackLevelIter()
      {
         delete fIter;
      }

      Bool_t next() override
      {
         fCurrent = fIter->Next();
         return fCurrent != nullptr;
      }

      Bool_t isfolder() override
      {
         return kFALSE;
      }

      TGo4LevelIter* subiterator() override
      {
         return nullptr;
      }

      const char *name() override
      {
         return fCurrent->GetName();
      }

      const char *info() override
      {
         return fCurrent->ClassName();
      }

      Int_t GetKind() override
      {
         return TGo4Access::kndObject;
      }

      const char *GetClassName() override
      {
         return fCurrent->ClassName();
      }
};

// ****************************************************************

TGo4HStackProxy::TGo4HStackProxy() :
   TGo4Proxy(),
   fHS(nullptr),
   fOwner(kFALSE)
{
}

TGo4HStackProxy::TGo4HStackProxy(THStack *hs, Bool_t owner) :
   TGo4Proxy(),
   fHS(hs),
   fOwner(owner)
{
}

TGo4HStackProxy::~TGo4HStackProxy()
{
   if (fOwner) delete fHS;
}

Int_t TGo4HStackProxy::GetObjectKind() const
{
   return fHS ? TGo4Access::kndFolder : TGo4Access::kndNone;
}

const char *TGo4HStackProxy::GetContainedClassName() const
{
   return fHS ? fHS->ClassName() : nullptr;
}

void TGo4HStackProxy::Initialize(TGo4Slot *slot)
{
   TGo4ObjectManager* om = slot->GetOM();
   if (om) om->RegisterObjectWith(fHS, slot);
}

void TGo4HStackProxy::Finalize(TGo4Slot *slot)
{
   TGo4ObjectManager* om = slot->GetOM();
   if (om) om->UnregisterObject(fHS, slot);
}

Bool_t TGo4HStackProxy::RemoveRegisteredObject(TObject *obj)
{
   if (obj == fHS) {
      fHS = nullptr;
      fOwner = kFALSE;
   }
   return kFALSE;
}

void TGo4HStackProxy::WriteData(TGo4Slot *slot, TDirectory* dir, Bool_t onlyobjs)
{
   const char *objname = nullptr;
   if (fHS)
      objname = fHS->GetName();

   if (!onlyobjs)
      slot->SetPar("HStackProxy::StackName", objname);

   if (!dir || !fHS) return;

   dir->cd();

   fHS->Write(objname);
}

void TGo4HStackProxy::ReadData(TGo4Slot *slot, TDirectory* dir)
{
   const char *objname = slot->GetPar("HStackProxy::StackName");
   if (!objname || !dir) return;

   dir->cd();

   AssignObject(slot, dir->Get(objname), kTRUE);
}

Bool_t TGo4HStackProxy::IsAcceptObject(TClass *cl) const
{
   return cl && cl->InheritsFrom(THStack::Class());
}

Bool_t TGo4HStackProxy::AssignObject(TGo4Slot *slot, TObject *obj, Bool_t owner)
{
   Finalize(slot);
   if (fHS && fOwner) delete fHS;

   fHS = dynamic_cast<THStack*> (obj);
   fOwner = owner && fHS;

   if (!fHS && obj && owner) delete obj;

   Initialize(slot);

   slot->ForwardEvent(slot, TGo4Slot::evObjAssigned);

   return fHS != nullptr;
}

TObject *TGo4HStackProxy::GetAssignedObject()
{
   return fHS;
}

std::unique_ptr<TGo4Access> TGo4HStackProxy::CreateAccess(THStack *hs, const char *name)
{
   if (!hs) return nullptr;
   if (!name || !*name)
      return std::make_unique<TGo4ObjectAccess>(hs);

   TObject *obj = hs->GetHists()->FindObject(name);

   return obj ? std::make_unique<TGo4ObjectAccess>(obj) : nullptr;
}

TGo4LevelIter* TGo4HStackProxy::ProduceIter(THStack *hs)
{
   return new TGo4HStackLevelIter(hs);
}
