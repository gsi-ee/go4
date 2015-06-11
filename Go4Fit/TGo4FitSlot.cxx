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

#include "TGo4FitSlot.h"

#include "Riostream.h"
#include "TClass.h"
#include "TH1.h"
#include "TSeqCollection.h"
#include "TObjArray.h"

TGo4FitSlot::TGo4FitSlot() :
   TGo4FitNamed(),
   fxClass(0),
   fiSaveFlag(0),
   fbOwned(kFALSE),
   fbNeeded(kFALSE),
   fxObject(0),
   fiSaveSlot(-1),
   fiSaveOwnership(-1)
{
}

TGo4FitSlot::TGo4FitSlot(TNamed* iOwner, TClass* iClass) :
   TGo4FitNamed(),
   fxClass(iClass),
   fiSaveFlag(0),
   fbOwned(kFALSE),
   fbNeeded(kFALSE),
   fxObject(0),
   fiSaveSlot(-1),
   fiSaveOwnership(-1)
{
   SetOwner(iOwner);
}

TGo4FitSlot::TGo4FitSlot(const char* iName, const char* iTitle,
                         TNamed* iOwner, TClass* iClass,
                         Bool_t iNeeded, TObject* iObject, Bool_t iOwned) :
    TGo4FitNamed(iName,iTitle, iOwner),
    fxClass(iClass),
    fiSaveFlag(0),
    fbOwned(iOwned),
    fbNeeded(iNeeded),
    fxObject(iObject),
    fiSaveSlot(-1),
    fiSaveOwnership(-1)
{
}

TGo4FitSlot::~TGo4FitSlot()
{
   if (fbOwned && fxObject)
       delete fxObject;
}

void TGo4FitSlot::SetDefaults(TNamed* iOwner, TClass* iClass)
{
   SetOwner(iOwner);
   fxClass = iClass;
}

Bool_t TGo4FitSlot::ConnectToSlot(TGo4FitSlot* slot)
{
  if (!CanConnectToSlot(slot)) return kFALSE;
  if (fbOwned && fxObject) delete fxObject;
  fxObject = slot;
  fbOwned = kFALSE;
  return kTRUE;
}

Bool_t TGo4FitSlot::CanConnectToSlot(TGo4FitSlot* slot)
{
  if ((slot==0) || (slot==this)) return kFALSE;
  if (!GetClass()->InheritsFrom(slot->GetClass())) return kFALSE;
  TGo4FitSlot* conn = slot;
  while ( (conn = conn->GetConnectedSlot())!=0)
    if (conn == this) return kFALSE;
  return kTRUE;
}

void TGo4FitSlot::ClearConnectionToSlot()
{
   if (IsConnectedToSlot()) {
     fxObject = 0;
     fbOwned = kFALSE;
   }
}

Bool_t TGo4FitSlot::IsSuitable(TObject* obj)
{
   if (obj==0) return kFALSE;
   if (IsConnectedToSlot()) return GetConnectedSlot()->IsSuitable(obj);
                       else return obj->InheritsFrom(GetClass());
}

Bool_t TGo4FitSlot::IsSuitableClass(TClass* cl)
{
   if (cl==0) return kFALSE;
   if (IsConnectedToSlot()) return GetConnectedSlot()->IsSuitableClass(cl);
                       else return cl->InheritsFrom(GetClass());
}

void TGo4FitSlot::ClearObject()
{
   fxObject = 0;
   fbOwned = kFALSE;
}

Bool_t TGo4FitSlot::SetObject(TObject* iObject, Bool_t iOwned, Bool_t CheckClass)
{
   if (IsConnectedToSlot())
     return GetConnectedSlot()->SetObject(iObject, iOwned, CheckClass);

   if (CheckClass && iObject)
     if (!iObject->InheritsFrom(GetClass())) return kFALSE;

   if (fbOwned && fxObject) delete fxObject;

   fxObject = iObject;
   fbOwned = iOwned;
   CheckOwnership();
   return kTRUE;
}

TObject* TGo4FitSlot::GetObject() const
{
   return IsConnectedToSlot() ? GetConnectedSlot()->GetObject() : fxObject;
}

const char* TGo4FitSlot::GetObjectName() const
{
   TObject* obj = GetObject();
   if (obj && obj->InheritsFrom(TNamed::Class())) return obj->GetName();
                                                  else return 0;
}

TObject* TGo4FitSlot::CloneObject(const char* newname)
{
   TObject* obj = GetObject();
   if (obj) return obj->Clone(newname);
       else return 0;
}

void TGo4FitSlot::Print(Option_t* option) const
{
   std::cout << "Slot: " << GetName() << " for class: " << fxClass->GetName()
        << "  needed:" << fbNeeded << "  owned:" << fbOwned;
   if (IsConnectedToSlot()) {
      std::cout << "  Connected to: " << std::endl << "  ";
     GetConnectedSlot()->Print(option);
   } else
   if (fxObject) {
      std::cout << " object: " << GetObjectName() << std::endl;
     if (((strcmp(option,"**")==0) && fbOwned) || (strcmp(option,"***")==0))
         fxObject->Print(option);
   } else
      std::cout << " no object. " << std::endl;
}

void TGo4FitSlot::CheckOwnership()
{
  if(fbOwned && !IsConnectedToSlot()) {
     TNamed* master = dynamic_cast<TNamed*> (GetOwner());
     TGo4FitNamed* slave = dynamic_cast<TGo4FitNamed*> (fxObject);
     if (master && slave) slave->SetOwner(master);
  }
}

Bool_t TGo4FitSlot::WillBeSaved()
{
   return (fiSaveFlag==1) || ((fiSaveFlag==0) && fbOwned);
}

void TGo4FitSlot::SetSaveSettings(Int_t save, Int_t ownership)
{
   fiSaveSlot = save;
   fiSaveOwnership = ownership;
}

Bool_t TGo4FitSlot::HasSaveSettings()
{
  return (fiSaveSlot>=0) && (fiSaveOwnership>=0);
}

void TGo4FitSlot::Streamer(TBuffer& b)
{
   if (b.IsReading()) {

     TGo4FitSlot::Class()->ReadBuffer(b, this);

     b >> fbNeeded;
     b >> fbOwned;
     Bool_t saveflag = kFALSE;
     b >> saveflag;

     if (saveflag) {
       b >> fxObject;
       if (fxObject->InheritsFrom(TH1::Class()))
          ((TH1*) fxObject)->SetDirectory(0);
       CheckOwnership();
     }
   } else {

     TGo4FitSlot::Class()->WriteBuffer(b, this);

     Bool_t saveflag = kFALSE;
     Bool_t saveown = kFALSE;
     if ((fiSaveSlot>=0) && (fiSaveOwnership>=0)) {
        saveflag = fiSaveSlot>0;
        saveown = fiSaveOwnership>0;
     } else {
        saveflag = (fxObject!=0) && (WillBeSaved() || IsConnectedToSlot());
        saveown = saveflag && GetOwned();
     }

     b << fbNeeded;
     b << saveown;
     b << saveflag;
     if (saveflag) b << fxObject;
   }

   fiSaveSlot = -1;
   fiSaveOwnership = -1;
}

//********************************************************************************

TGo4FitSlotList::TGo4FitSlotList()
{
   fxSlotList = 0;
   fbUpdateSlotList = kFALSE;
}

TGo4FitSlotList::~TGo4FitSlotList()
{
   if (fxSlotList) { delete fxSlotList; fxSlotList = 0; }
}

void TGo4FitSlotList::FillSlotList(TSeqCollection* lst)
{
   if (lst==0) return;
}

void TGo4FitSlotList::SetUpdateSlotList()
{
   fbUpdateSlotList = kTRUE;
}

const TObjArray* TGo4FitSlotList::GetSlotList(Bool_t ForceUpdate)
{
   if (fxSlotList==0) {
     fxSlotList = new TObjArray(10);
     FillSlotList(fxSlotList);
   } else
   if (fbUpdateSlotList || ForceUpdate) {
     fxSlotList->Clear();
     FillSlotList(fxSlotList);
   }
   fbUpdateSlotList = kFALSE;
   return fxSlotList;
}

Int_t TGo4FitSlotList::NumSlots()
{
  return GetSlotList()->GetLast()+1;
}

TGo4FitSlot* TGo4FitSlotList::GetSlot(Int_t nslot)
{
   const TObjArray* lst = GetSlotList();
   return (nslot>=0) && (nslot<=lst->GetLast()) ? dynamic_cast<TGo4FitSlot*> (lst->At(nslot)) : 0;
}

TGo4FitSlot* TGo4FitSlotList::FindSlot(const char* FullSlotName)
{
   const TObjArray* lst = GetSlotList();
   for(Int_t i=0;i<=lst->GetLast();i++) {
      TGo4FitSlot* slot = dynamic_cast<TGo4FitSlot*> (lst->At(i));
      if (slot && (strcmp(slot->GetFullName(), FullSlotName)==0)) return slot;
   }
   return 0;
}

Bool_t TGo4FitSlotList::ConnectSlots(TGo4FitSlot* slot1, TGo4FitSlot* slot2)
{
  if ((slot1==0) || (slot2==0)) return kFALSE;
  return slot1->ConnectToSlot(slot2);
}

Bool_t TGo4FitSlotList::ConnectSlots(const char* Slot1FullName, const char* Slot2FullName)
{
   return ConnectSlots(FindSlot(Slot1FullName), FindSlot(Slot2FullName));
}

TGo4FitSlot* TGo4FitSlotList::SetObject(TObject* obj, Bool_t iOwned)
{
   if (obj==0) return 0;

   const TObjArray* lst = GetSlotList();

   for(Int_t i=0;i<=lst->GetLast();i++) {
      TGo4FitSlot* slot = dynamic_cast<TGo4FitSlot*> (lst->At(i));
      if (slot==0) continue;

      if (slot->IsEmpty() && slot->IsSuitable(obj)) {
        slot->SetObject(obj, iOwned);
        return slot;
      }
   }
   return 0;
}

TGo4FitSlot* TGo4FitSlotList::SetObject(const char* PlaceName, TObject* obj, Bool_t iOwned)
{
   if (obj==0) return 0;

   if (PlaceName==0) return SetObject(obj, iOwned);

   const TObjArray* lst = GetSlotList();

   TGo4FitSlot* firstempty = 0, *last = 0;
   Int_t count = 0;

   for(Int_t i=0;i<=lst->GetLast();i++) {
      TGo4FitSlot* slot = dynamic_cast<TGo4FitSlot*> (lst->At(i));
      if ((slot==0) || !slot->IsSuitable(obj)) continue;

      if (strcmp(slot->GetFullName(),PlaceName)==0) { firstempty = slot; count = 1; break; }

      if (strcmp(slot->GetOwnerFullName(), PlaceName)==0) {
          count++;
          last = slot;
          if (slot->IsEmpty() && (firstempty==0)) firstempty = slot;
        }
   }

   if (firstempty!=0) {
      firstempty->SetObject(obj, iOwned);
      return firstempty;
   } else
   if ((count==1) && (last!=0)) {
      last->SetObject(obj, iOwned);
      return last;
   }

   return 0;
}

TGo4FitSlot* TGo4FitSlotList::IsObjectInSlots(TObject* obj)
{
   if (obj==0) return 0;

   const TObjArray* lst = GetSlotList();

   TGo4FitSlot* last = 0;

   for(Int_t i=0;i<=lst->GetLast();i++) {
      TGo4FitSlot* slot = dynamic_cast<TGo4FitSlot*> (lst->At(i));
      if (slot==0) continue;
      if (slot->GetObject()==obj) {
        last=slot;
        if (slot->GetOwned()) return slot;
      }
   }
   return last;
}

Bool_t TGo4FitSlotList::CheckObjects(Bool_t MakeOut)
{
   Bool_t res = kTRUE;
   const TObjArray* lst = GetSlotList();
   for(Int_t i=0;i<=lst->GetLast();i++) {
      TGo4FitSlot* slot = dynamic_cast<TGo4FitSlot*> (lst->At(i));
      if (slot==0) continue;
      if (slot->IsRequired()) {
         if (MakeOut)
            std::cout << "Required data not provided" << std::endl <<
                   "   Name: " << slot->GetName() << std::endl <<
                   "   Class: " << slot->GetClass()->GetName() << std::endl <<
                   "   Description: " << slot->GetTitle() << std::endl <<
                   "   For object: " << slot->GetOwnerFullName() << std::endl;
         res = kFALSE;
       }
   }
   return res;
}

Bool_t TGo4FitSlotList::IsEmptySlots()
{
   TObjArray list;
   const TObjArray* lst = GetSlotList();
   for(Int_t i=0;i<=lst->GetLast();i++) {
      TGo4FitSlot* slot = dynamic_cast<TGo4FitSlot*> (lst->At(i));
      if (slot==0) continue;
      if (slot->IsEmpty()) return kTRUE;
   }
   return kFALSE;
}


void TGo4FitSlotList::ClearObjects(const char* PlaceName, Bool_t NonOwned)
{
   const TObjArray* lst = GetSlotList();

   for(Int_t i=0;i<=lst->GetLast();i++) {
      TGo4FitSlot* slot = dynamic_cast<TGo4FitSlot*> (lst->At(i));
      if (slot==0) continue;

      if ((PlaceName!=0) &&
         (strcmp(slot->GetFullName(),PlaceName)!=0) &&
         (strcmp(slot->GetOwnerFullName(), PlaceName)==0)) continue;

      ClearSlot(slot, NonOwned);
   }
}

void TGo4FitSlotList::ClearSlot(TGo4FitSlot* slot, Bool_t NonOwned)
{
   if (slot==0) return;

   while(slot->GetConnectedSlot())
     slot = slot->GetConnectedSlot();

   TObject* obj = slot->GetObject();
   Bool_t owned = slot->GetOwned();
   if (obj==0) return;
   if (NonOwned || !owned) {
      if (!owned) slot->Clear();
      return;
   }

   for(Int_t i=0;i<NumSlots();i++) {
      TGo4FitSlot* sl = GetSlot(i);
      if ((sl==0) || sl->IsConnectedToSlot() || (slot==sl)) continue;
      if (sl->GetObject()==obj) {
         sl->SetOwned(kTRUE);
         slot->SetOwned(kFALSE);
         slot->Clear();
         return;
      }
   }
   slot->Clear();
}

void TGo4FitSlotList::SetSaveFlagForObjects(Int_t iSaveFlag, const char* PlaceName)
{
   const TObjArray* lst = GetSlotList();

   for(Int_t i=0;i<=lst->GetLast();i++) {
      TGo4FitSlot* slot = dynamic_cast<TGo4FitSlot*> (lst->At(i));
      if (slot==0) continue;

      if ((PlaceName!=0) &&
         (strcmp(slot->GetFullName(),PlaceName)!=0) &&
         (strcmp(slot->GetOwnerFullName(), PlaceName)==0)) continue;

      slot->SetSaveFlag(iSaveFlag);
   }
}

void TGo4FitSlotList::CheckDuplicatesOnSlot()
{
   const TObjArray* lst = GetSlotList();

   for(Int_t n1=lst->GetLast();n1>0;n1--) {
      TGo4FitSlot* slot1 = dynamic_cast<TGo4FitSlot*> (lst->At(n1));
      if ((slot1==0) || !slot1->GetOwned() || slot1->IsConnectedToSlot()) continue;
      for(Int_t n2=n1-1;n2>=0;n2--) {
         TGo4FitSlot* slot2 = dynamic_cast<TGo4FitSlot*> (lst->At(n2));
         if ((slot2==0) || !slot2->GetOwned() || slot2->IsConnectedToSlot()) continue;
         if ((void*)(slot1->GetObject())==(void*)(slot2->GetObject()))
           slot2->SetOwned(kFALSE);
      }
   }
}

void TGo4FitSlotList::PrepareSlotsForWriting()
{
   const TObjArray* lst = GetSlotList();

   for(Int_t n1=0; n1<=lst->GetLast(); n1++) {
     TGo4FitSlot* slot1 = dynamic_cast<TGo4FitSlot*> (lst->At(n1));
     if (slot1) slot1->SetSaveSettings(-1, -1);
   }

   for(Int_t n1=0; n1<=lst->GetLast(); n1++) {
       TGo4FitSlot* slot1 = dynamic_cast<TGo4FitSlot*> (lst->At(n1));
       if ((slot1==0) || slot1->HasSaveSettings()) continue;

       if (slot1->IsConnectedToSlot()) {
         slot1->SetSaveSettings(kTRUE, kFALSE);
         continue;
       }

       TObject* saveobj = slot1->GetObject();

       if (saveobj==0) {
         slot1->SetSaveSettings(kFALSE, kFALSE);
         continue;
       }

       Bool_t isobjsaved = kFALSE;
       Bool_t isobjowned = kFALSE;

       for(Int_t n2=0; n2<=lst->GetLast(); n2++) {
         TGo4FitSlot* slot2 = dynamic_cast<TGo4FitSlot*> (lst->At(n2));
         if ((slot2==0) || slot2->IsConnectedToSlot() ||
             (slot2->GetObject()!=saveobj)) continue;

         if (slot2->WillBeSaved()) {
           isobjsaved = kTRUE;
           if (slot2->GetOwned()) {
             if (isobjowned) slot2->SetOwned(kFALSE);
                        else isobjowned = kTRUE;
           }
         }
       }

       for(Int_t n2=0; n2<=lst->GetLast(); n2++) {
         TGo4FitSlot* slot2 = dynamic_cast<TGo4FitSlot*> (lst->At(n2));
         if ((slot2==0) || slot2->IsConnectedToSlot() ||
             (slot2->GetObject()!=saveobj)) continue;

         if (isobjsaved)
           if (slot2->GetOwned()) slot2->SetSaveSettings(kTRUE, kTRUE);
                             else slot2->SetSaveSettings(kTRUE, kFALSE);
           else slot2->SetSaveSettings(kFALSE, kFALSE);
       }

       if (isobjsaved && !isobjowned)
         slot1->SetSaveSettings(kTRUE, kTRUE);
   }
}
