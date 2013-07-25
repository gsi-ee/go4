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

#include "TGo4ObjectProxy.h"

#include "TH1.h"
#include "TDirectory.h"
#include "TClass.h"
#include "TTree.h"

#include "TGo4Slot.h"
#include "TGo4Picture.h"
#include "TGo4ObjectManager.h"

TGo4ObjectAccess::TGo4ObjectAccess(TObject* obj) :
   TGo4Access(),
   fObject(obj)
{
}

Bool_t TGo4ObjectAccess::CanGetObject() const
{
   return kTRUE;
}

Bool_t TGo4ObjectAccess::GetObject(TObject* &obj, Bool_t &owner) const
{
   obj = fObject;
   owner = kFALSE;
   return kTRUE;
}

TClass* TGo4ObjectAccess::GetObjectClass() const
{
   return fObject->IsA();
}

const char* TGo4ObjectAccess::GetObjectName() const
{
   return fObject->GetName();
}

const char* TGo4ObjectAccess::GetObjectClassName() const
{
   return fObject->ClassName();
}

// ********************************************************************

TGo4ObjectProxy::TGo4ObjectProxy() :
   TGo4Proxy(),
   fObject(0),
   fOwner(kFALSE)
{
}

TGo4ObjectProxy::TGo4ObjectProxy(TObject* obj, Bool_t owner) :
   TGo4Proxy(),
   fObject(obj),
   fOwner(owner)
{
   if ((fObject!=0) && fOwner && fObject->InheritsFrom(TH1::Class()))
     ((TH1*) fObject)->SetDirectory(0);
}

TGo4ObjectProxy::~TGo4ObjectProxy()
{
   if (fOwner) delete fObject;
}

void TGo4ObjectProxy::Initialize(TGo4Slot* slot)
{
   TGo4ObjectManager* om = slot->GetOM();
   if (om!=0) om->RegisterObjectWith(fObject, slot);
}

void TGo4ObjectProxy::Finalize(TGo4Slot* slot)
{
   TGo4ObjectManager* om = slot->GetOM();
   if (om!=0) om->UnregisterObject(fObject, slot);
}

Bool_t TGo4ObjectProxy::RemoveRegisteredObject(TObject* obj)
{
   if (fObject==obj) {
//      std::cout << "object removed from TGo4ObjectProxy  "
//           << obj << "   name = " << (obj ? obj->GetName() : "null") << std::endl;
      fObject = 0;
      fOwner = kFALSE;
   }
   return kFALSE;
}

TGo4Access* TGo4ObjectProxy::MakeProxy(const char* name)
{
   if (fObject==0) return 0;
   if ((name==0) || (*name==0)) return new TGo4ObjectAccess(fObject);
   return 0;
}

void TGo4ObjectProxy::WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs)
{
   const char* objname = 0;
   if (fObject!=0)
      objname = fObject->GetName();

   if (!onlyobjs)
     slot->SetPar("ObjectProxy::ObjName", objname);

   if ((dir==0) || (fObject==0)) return;

   dir->cd();

   if (fObject->InheritsFrom(TH1::Class())) {
      TH1* h1 = (TH1*) fObject;
      TDirectory* olddir = h1->GetDirectory();
      h1->Write(objname);
      h1->SetDirectory(olddir);
   } else {
     fObject->Write(objname);
   }
}

void TGo4ObjectProxy::ReadData(TGo4Slot* slot, TDirectory* dir)
{
   const char* objname = slot->GetPar("ObjectProxy::ObjName");
   if ((objname==0) || (dir==0)) return;

   dir->cd();

   AssignObject(slot, dir->Get(objname), kTRUE);
}

Int_t TGo4ObjectProxy::GetObjectKind()
{
   return (fObject!=0) ? TGo4Access::kndObject : TGo4Access::kndNone;
}

const char* TGo4ObjectProxy::GetContainedClassName()
{
   return (fObject!=0) ? fObject->ClassName() : 0;
}

const char* TGo4ObjectProxy::GetContainedObjectInfo()
{
   return (fObject!=0) ? fObject->GetTitle() : 0;
}

Int_t TGo4ObjectProxy::GetObjectSizeInfo()
{
   Int_t sz = TGo4Proxy::GetObjectSizeInfo();
   if (fObject!=0) sz = DefineObjectSize(fObject);
   return sz;
}

Bool_t TGo4ObjectProxy::IsAcceptObject(TClass* cl)
{
   return (cl!=0) && cl->InheritsFrom(TObject::Class());
}

Bool_t TGo4ObjectProxy::AssignObject(TGo4Slot* slot, TObject* obj, Bool_t owner)
{
   Finalize(slot);
   if ((fObject!=0) && fOwner) delete fObject;
   fObject = obj;
   fOwner = owner;

   if ((fObject!=0) && fOwner && fObject->InheritsFrom(TH1::Class()))
     ((TH1*) fObject)->SetDirectory(0);

   Initialize(slot);

   slot->ForwardEvent(slot, TGo4Slot::evObjAssigned);

   return kTRUE;
}

TObject* TGo4ObjectProxy::GetAssignedObject()
{
   return fObject;
}


Long_t TGo4ObjectProxy::DefineObjectSize(TObject* obj)
{
   if (obj==0) return 0;

   Long_t sz = obj->IsA()->Size();

   if (obj->InheritsFrom(TH1::Class())) {
      TH1* histo = dynamic_cast<TH1*> (obj);
      Int_t nbins = histo->GetNbinsX()+2;
      if (histo->GetDimension()>1)
         nbins = nbins*(histo->GetNbinsY()+2);
      if (histo->GetDimension()>2)
         nbins = nbins * (histo->GetNbinsZ()+2);
      Int_t binsize = 1;
      if (strchr(histo->ClassName(),'S')!=0) binsize = sizeof(Short_t); else
      if (strchr(histo->ClassName(),'D')!=0) binsize = sizeof(Double_t); else
      if (strchr(histo->ClassName(),'F')!=0) binsize = sizeof(Float_t); else
      if (strchr(histo->ClassName(),'I')!=0) binsize = sizeof(Int_t); else
      if (strchr(histo->ClassName(),'C')!=0) binsize = sizeof(Char_t);
      sz += binsize * nbins;
   } else
   if (obj->InheritsFrom(TTree::Class())) {
      TTree* t = (TTree*) obj;
      sz += t->GetZipBytes();
   } else
   if (obj->InheritsFrom(TGo4Picture::Class())) {
      TGo4Picture* pic = (TGo4Picture*) obj;
      sz = pic->GetTotalSize();
   }

   return sz;

}
