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

#include "TGo4LinkProxy.h"

#include "TGo4ObjectManager.h"

TGo4LinkProxy::TGo4LinkProxy() :
   TGo4Proxy()
{
}

TGo4LinkProxy::TGo4LinkProxy(TGo4Slot* linkedslot) :
   TGo4Proxy(),
   fLink(linkedslot)
{
}

TGo4LinkProxy::~TGo4LinkProxy()
{
}

void TGo4LinkProxy::Initialize(TGo4Slot* slot)
{
   auto om = slot->GetOM();
   if (om)
     om->RegisterLink(fLink, slot);
}

void TGo4LinkProxy::Finalize(TGo4Slot* slot)
{
   auto om = slot->GetOM();
   if (om) {
      // this is workaround a problem, that in TGo4Slot destructor CleanProxy
      // called before Event(this, evDelete)
      // For that situation we retranslate evDelete message before link will dissaper from OM
      if (slot->DoingDelete())
         om->RetranslateEvent(slot, TGo4Slot::evDelete);

      om->UnregisterLink(slot);
   }
}

TGo4Access* TGo4LinkProxy::ProvideAccess(const char *name)
{
   return fLink ? fLink->ProvideSlotAccess(name) : nullptr;
}

TGo4LevelIter* TGo4LinkProxy::MakeIter()
{
   return fLink ? fLink->MakeLevelIter() : nullptr;
}

void TGo4LinkProxy::WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs)
{
   if (!onlyobjs) {
      TString linkname;
      if (fLink)
        fLink->ProduceFullName(linkname);

      slot->SetPar("LinkProxy::LinkName", linkname.Length() > 0 ? linkname.Data() : nullptr);
   }
}

void TGo4LinkProxy::ReadData(TGo4Slot* slot, TDirectory* dir)
{
   const char *linkname = slot->GetPar("LinkProxy::LinkName");
   if (linkname)
     fLink = slot->GetOM()->GetSlot(linkname);
}

Int_t TGo4LinkProxy::GetObjectKind()
{
   return fLink ? fLink->GetSlotKind() : TGo4Access::kndNone;
}

const char *TGo4LinkProxy::GetContainedClassName()
{
   return fLink ? fLink->GetSlotClassName() : nullptr;
}

Bool_t TGo4LinkProxy::IsAcceptObject(TClass* cl)
{
   return fLink ? fLink->IsAcceptObject(cl) : kFALSE;
}

Bool_t TGo4LinkProxy::AssignObject(TGo4Slot* slot, TObject *obj, Bool_t owner)
{
   return fLink ? fLink->AssignObject(obj, owner) : kFALSE;
}

TObject* TGo4LinkProxy::GetAssignedObject()
{
   return fLink ? fLink->GetAssignedObject() : nullptr;
}

Bool_t TGo4LinkProxy::ProcessEvent(TGo4Slot* slot, TGo4Slot* source, Int_t id, void* param)
{
    if ((id == TGo4Slot::evDelete) && (source == fLink)) {
       // next line very important.
       // If master slot says that it will be deleted, we just
       // brake dependency from it and "forgot" that it is existing
       // As a result, access to master object will be canceled
       // After we delete slot with our link proxy.
       // For instance, if marker is assigned to histogram, and histogram
       // is deleted (file closed), marker cannot access histogram pointer
       // and will be reassigned to null histogram even when master slot is
       // able to provide histogram pointer.
       fLink = nullptr;
       slot->Delete();
//       delete slot;
       // we delete slot with timer afterwards

//       TGo4ObjectManager* om = slot->GetOM();
       // om->UnregisterLink(slot);
//       om->DeleteObject(slot);

//       slot->ForwardEvent(slot, TGo4Slot::evObjDeleted);

       return kFALSE;
    }

    // emulate that message come from this slot, not from master link
    slot->ForwardEvent(slot, id, param);

    return kFALSE;
}
