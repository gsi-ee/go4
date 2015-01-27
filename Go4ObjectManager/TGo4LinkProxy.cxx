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

#include "TGo4LinkProxy.h"

#include "TH1.h"
#include "TString.h"

#include "TGo4Slot.h"
#include "TGo4ObjectManager.h"

TGo4LinkProxy::TGo4LinkProxy() :
   TGo4Proxy(),
   fLink(0)
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
   TGo4ObjectManager* om = slot->GetOM();
   if (om!=0)
     om->RegisterLink(fLink, slot);
}

void TGo4LinkProxy::Finalize(TGo4Slot* slot)
{
   TGo4ObjectManager* om = slot->GetOM();
   if (om!=0) {
      // this is workaround a problem, that in TGo4Slot destructor CleanProxy
      // called before Event(this, evDelete)
      // For that situation we retranslate evDelete message before link will dissaper from OM
      if (slot->DoingDelete())
         om->RetranslateEvent(slot, TGo4Slot::evDelete);

      om->UnregisterLink(slot);
   }
}

TGo4Access* TGo4LinkProxy::ProvideAccess(const char* name)
{
   return (fLink!=0)  ? fLink->ProvideSlotAccess(name) : 0;
}

TGo4LevelIter* TGo4LinkProxy::MakeIter()
{
   return (fLink!=0)  ? fLink->MakeLevelIter() : 0;
}

void TGo4LinkProxy::WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs)
{
   if (!onlyobjs) {
      TString linkname;
      if (fLink!=0)
        fLink->ProduceFullName(linkname);

      slot->SetPar("LinkProxy::LinkName", linkname.Length()>0 ? linkname.Data() : 0);
   }
}

void TGo4LinkProxy::ReadData(TGo4Slot* slot, TDirectory* dir)
{
   const char* linkname = slot->GetPar("LinkProxy::LinkName");
   if (linkname!=0)
     fLink = slot->GetOM()->GetSlot(linkname);

}

Int_t TGo4LinkProxy::GetObjectKind()
{
   return (fLink!=0) ? fLink->GetSlotKind() : TGo4Access::kndNone;
}

const char* TGo4LinkProxy::GetContainedClassName()
{
   return (fLink!=0) ? fLink->GetSlotClassName() : 0;
}

Bool_t TGo4LinkProxy::IsAcceptObject(TClass* cl)
{
   return (fLink!=0) ? fLink->IsAcceptObject(cl) : kFALSE;
}

Bool_t TGo4LinkProxy::AssignObject(TGo4Slot* slot, TObject* obj, Bool_t owner)
{
   return fLink==0 ? kFALSE : fLink->AssignObject(obj, owner);
}

TObject* TGo4LinkProxy::GetAssignedObject()
{
   return fLink==0 ? 0 : fLink->GetAssignedObject();
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
       fLink = 0;
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
