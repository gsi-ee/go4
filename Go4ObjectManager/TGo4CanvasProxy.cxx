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

#include "TGo4CanvasProxy.h"

#include "TObjArray.h"
#include "TClass.h"
#include "TDirectory.h"
#include "TFrame.h"
#include "TPaveText.h"
#include "TObjString.h"
#include "THStack.h"
#include "TH1.h"
#include "TCanvas.h"

#include "TGo4ObjectProxy.h"
#include "TGo4HStackProxy.h"
#include "TGo4ObjectManager.h"

class TGo4CanvasLevelIter : public TGo4LevelIter {
   protected:
      TIterator*     fIter{nullptr};     //!
      TObject*       fCurrent{nullptr};  //!
      TObjArray      fStacks;   //!
   public:
      TGo4CanvasLevelIter(TPad* pad) :
         TGo4LevelIter(),
         fIter(nullptr),
         fCurrent(nullptr),
         fStacks()
      {
         fIter = pad->GetListOfPrimitives()->MakeIterator();
         fStacks.SetOwner(kTRUE);
      }

      virtual ~TGo4CanvasLevelIter()
      {
         delete fIter;
      }

      Bool_t next() override
      {
         bool skip = false;
         do {
           skip = false;
           fCurrent = fIter->Next();

           if (!fCurrent) break;

           TFrame* fr = dynamic_cast<TFrame*>(fCurrent);
           if (fr) { skip = true; continue; }

           TPaveText* ptxt = dynamic_cast<TPaveText*> (fCurrent);
           if (ptxt && (strcmp(ptxt->GetName(),"title") == 0)) { skip = true; continue; }

           if (dynamic_cast<TPave*>(fCurrent)) { skip = true; continue; }

           if (!fCurrent->GetName()) { skip = true; continue; }

           if (strlen(fCurrent->GetName()) == 0) { skip = true; continue; }

           if (strcmp(fCurrent->GetName(), fCurrent->ClassName()) == 0) { skip = true; continue; }

           // this is axis object for drawing THStack in the canvas
           if (fStacks.FindObject(fCurrent->GetName()) && fCurrent->InheritsFrom(TH1F::Class())) { skip = true; continue; }

         } while(skip);

         if (fCurrent && fCurrent->InheritsFrom(THStack::Class()))
            fStacks.Add(new TObjString(fCurrent->GetName()));

         return fCurrent != nullptr;
      }

      Bool_t isfolder() override
      {
         return dynamic_cast<TPad*>(fCurrent) ||
                dynamic_cast<THStack*>(fCurrent);
      }

      TGo4LevelIter* subiterator() override
      {
         TPad* subpad = dynamic_cast<TPad*>(fCurrent);
         if (subpad) return new TGo4CanvasLevelIter(subpad);
         THStack *hs = dynamic_cast<THStack*> (fCurrent);
         if (hs) return TGo4HStackProxy::ProduceIter(hs);
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
         return isfolder() ? TGo4Access::kndFolder : TGo4Access::kndObject;
      }

      const char *GetClassName() override
      {
         return fCurrent->ClassName();
      }
};

// ****************************************************************

TGo4CanvasProxy::TGo4CanvasProxy() :
   TGo4Proxy()
{
}

TGo4CanvasProxy::TGo4CanvasProxy(TCanvas *c, Bool_t owner) :
   TGo4Proxy(),
   fCanvas(c),
   fOwner(owner)
{
}

TGo4CanvasProxy::~TGo4CanvasProxy()
{
   if (fOwner) delete fCanvas;
}

Int_t TGo4CanvasProxy::GetObjectKind() const
{
   return fCanvas ? TGo4Access::kndFolder : TGo4Access::kndNone;
}

const char *TGo4CanvasProxy::GetContainedClassName() const
{
   return fCanvas ? fCanvas->ClassName() : nullptr;
}

void TGo4CanvasProxy::Initialize(TGo4Slot *slot)
{
   auto om = slot->GetOM();
   if (om) om->RegisterObjectWith(fCanvas, slot);
}

void TGo4CanvasProxy::Finalize(TGo4Slot *slot)
{
   auto om = slot->GetOM();
   if (om) om->UnregisterObject(fCanvas, slot);
}

Bool_t TGo4CanvasProxy::RemoveRegisteredObject(TObject *obj)
{
   if (obj==fCanvas) {
      fCanvas = nullptr;
      fOwner = kFALSE;
   }
   return kFALSE;
}

void TGo4CanvasProxy::WriteData(TGo4Slot *slot, TDirectory* dir, Bool_t onlyobjs)
{
   const char *objname = nullptr;
   if (fCanvas)
      objname = fCanvas->GetName();

   if (!onlyobjs)
     slot->SetPar("CanvasProxy::CanvasName", objname);

   if (!dir || !fCanvas) return;

   dir->cd();

   fCanvas->Write(objname);
}

void TGo4CanvasProxy::ReadData(TGo4Slot *slot, TDirectory* dir)
{
   const char *objname = slot->GetPar("CanvasProxy::CanvasName");
   if (!objname || !dir) return;

   dir->cd();

   AssignObject(slot, dir->Get(objname), kTRUE);
}

Bool_t TGo4CanvasProxy::IsAcceptObject(TClass *cl) const
{
   return cl && cl->InheritsFrom(TCanvas::Class());
}

Bool_t TGo4CanvasProxy::AssignObject(TGo4Slot *slot, TObject *obj, Bool_t owner)
{
   Finalize(slot);
   if (fCanvas && fOwner) delete fCanvas;

   fCanvas = dynamic_cast<TCanvas*> (obj);
   fOwner = owner && fCanvas;

   if (!fCanvas && obj && owner) delete obj;

   Initialize(slot);

   slot->ForwardEvent(slot, TGo4Slot::evObjAssigned);

   return fCanvas;
}

TObject* TGo4CanvasProxy::GetAssignedObject()
{
   return fCanvas;
}

std::unique_ptr<TGo4Access> TGo4CanvasProxy::CreateAccess(TCanvas *canv, const char *name)
{
   if (!canv) return nullptr;
   if (!name || !*name)
      return std::make_unique<TGo4ObjectAccess>(canv);

   TPad* curpad = canv;
   const char *curname = name;

   while (curpad) {
      const char *slash = strchr(curname, '/');
      UInt_t len = slash ? slash - curname : strlen(curname);
      TIter iter(curpad->GetListOfPrimitives());
      TObject *obj = nullptr;
      while ((obj = iter()) != nullptr)
         if ((strlen(obj->GetName()) == len) &&
             (strncmp(obj->GetName(), curname, len) == 0)) break;
      if (!obj) return nullptr;

      if (!slash)
         return std::make_unique<TGo4ObjectAccess>(obj);

      curname = slash+1;

      curpad = dynamic_cast<TPad*>(obj);

      if (!curpad) {
         THStack *hs = dynamic_cast<THStack*> (obj);
         if (hs)
            return TGo4HStackProxy::CreateAccess(hs, curname);
      }
   }

   return nullptr;
}

TGo4LevelIter* TGo4CanvasProxy::ProduceIter(TCanvas *canv)
{
   return new TGo4CanvasLevelIter(canv);
}
