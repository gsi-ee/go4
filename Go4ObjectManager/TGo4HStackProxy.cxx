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

#include "TGo4HStackProxy.h"

#include "TROOT.h"
#include "TObjArray.h"
#include "TClass.h"
#include "TFrame.h"
#include "TPaveText.h"
#include "TObjString.h"
#include "THStack.h"
#include "TH1.h"

#include "TGo4Slot.h"
#include "TGo4ObjectProxy.h"
#include "TGo4ObjectManager.h"

class TGo4HStackLevelIter : public TGo4LevelIter {
   protected:
      TIterator*     fIter;     //!
      TObject*       fCurrent;  //!
   public:
      TGo4HStackLevelIter(THStack* hs) :
         TGo4LevelIter(),
         fIter(0),
         fCurrent(0)
      {
         fIter = hs->GetHists()->MakeIterator();
      }

      virtual ~TGo4HStackLevelIter()
      {
         delete fIter;
      }

      virtual Bool_t next()
      {
         fCurrent = fIter->Next();
         return (fCurrent!=0);
      }

      virtual Bool_t isfolder()
      {
         return kFALSE;
      }

      virtual TGo4LevelIter* subiterator()
      {
         return 0;
      }

      virtual const char* name()
      {
         return fCurrent->GetName();
      }

      virtual const char* info()
      {
         return fCurrent->ClassName();
      }

      virtual Int_t GetKind()
      {
         return TGo4Access::kndObject;
      }

      virtual const char* GetClassName()
      {
         return fCurrent->ClassName();
      }
};

// ****************************************************************

TGo4HStackProxy::TGo4HStackProxy() :
   TGo4Proxy(),
   fHS(0),
   fOwner(kFALSE)
{
}

TGo4HStackProxy::TGo4HStackProxy(THStack* hs, Bool_t owner) :
   TGo4Proxy(),
   fHS(hs),
   fOwner(owner)
{
}

TGo4HStackProxy::~TGo4HStackProxy()
{
   if (fOwner) delete fHS;
}

Int_t TGo4HStackProxy::GetObjectKind()
{
   return (fHS!=0) ? TGo4Access::kndFolder : TGo4Access::kndNone;
}

const char* TGo4HStackProxy::GetContainedClassName()
{
   return fHS ? fHS->ClassName() : 0;
}

void TGo4HStackProxy::Initialize(TGo4Slot* slot)
{
   TGo4ObjectManager* om = slot->GetOM();
   if (om!=0) om->RegisterObjectWith(fHS, slot);
}

void TGo4HStackProxy::Finalize(TGo4Slot* slot)
{
   TGo4ObjectManager* om = slot->GetOM();
   if (om!=0) om->UnregisterObject(fHS, slot);
}

Bool_t TGo4HStackProxy::RemoveRegisteredObject(TObject* obj)
{
   if (obj==fHS) {
      fHS = 0;
      fOwner = kFALSE;
   }
   return kFALSE;
}

void TGo4HStackProxy::WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs)
{
   const char* objname = 0;
   if (fHS!=0)
      objname = fHS->GetName();

   if (!onlyobjs)
      slot->SetPar("HStackProxy::StackName", objname);

   if ((dir==0) || (fHS==0)) return;

   dir->cd();

   fHS->Write(objname);
}

void TGo4HStackProxy::ReadData(TGo4Slot* slot, TDirectory* dir)
{
   const char* objname = slot->GetPar("HStackProxy::StackName");
   if ((objname==0) || (dir==0)) return;

   dir->cd();

   AssignObject(slot, dir->Get(objname), kTRUE);
}

Bool_t TGo4HStackProxy::IsAcceptObject(TClass* cl)
{
   return (cl!=0) && cl->InheritsFrom(THStack::Class());
}

Bool_t TGo4HStackProxy::AssignObject(TGo4Slot* slot, TObject* obj, Bool_t owner)
{
   Finalize(slot);
   if ((fHS!=0) && fOwner) delete fHS;

   fHS = dynamic_cast<THStack*> (obj);
   fOwner = owner && (fHS!=0);

   if ((fHS==0) && (obj!=0) && owner) delete obj;

   Initialize(slot);

   slot->ForwardEvent(slot, TGo4Slot::evObjAssigned);

   return fHS!=0;
}

TObject* TGo4HStackProxy::GetAssignedObject()
{
   return fHS;
}

TGo4Access* TGo4HStackProxy::CreateAccess(THStack* hs, const char* name)
{
   if (hs==0) return 0;
   if ((name==0) || (*name==0)) return new TGo4ObjectAccess(hs);

   TObject* obj = hs->GetHists()->FindObject(name);

   return obj ? new TGo4ObjectAccess(obj) : 0;
}

TGo4LevelIter* TGo4HStackProxy::ProduceIter(THStack* hs)
{
   return new TGo4HStackLevelIter(hs);
}
