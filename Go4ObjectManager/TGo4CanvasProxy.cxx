#include "TGo4CanvasProxy.h"

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

class TGo4CanvasLevelIter : public TGo4LevelIter {
   protected:
      TIterator*     fIter;     //!
      TObject*       fCurrent;  //!
      TObjArray      fStacks;   //!
   public:
      TGo4CanvasLevelIter(TPad* pad) :
         TGo4LevelIter(),
         fIter(0),
         fCurrent(0),
         fStacks()
      {
         fIter = pad->GetListOfPrimitives()->MakeIterator();
         fStacks.SetOwner(kTRUE);
      }

      virtual ~TGo4CanvasLevelIter()
      {
         delete fIter;
      }

      virtual Bool_t next()
      {
         bool skip = false;
         do {
           skip = false;
           fCurrent = fIter->Next();

           if (fCurrent==0) break;

           TFrame* fr = dynamic_cast<TFrame*>(fCurrent);
           if (fr!=0) { skip = true; continue; }

           TPaveText* ptxt = dynamic_cast<TPaveText*> (fCurrent);
           if ((ptxt!=0) && (strcmp(ptxt->GetName(),"title")==0)) { skip = true; continue; }

           if (dynamic_cast<TPave*>(fCurrent) != 0) { skip = true; continue; }

           if (fCurrent->GetName()==0) { skip = true; continue; }

           if (strlen(fCurrent->GetName())==0) { skip = true; continue; }

           if (strcmp(fCurrent->GetName(), fCurrent->ClassName())==0) { skip = true; continue; }

           // this is axis object for drawing THStack in the canvas
           if (fStacks.FindObject(fCurrent->GetName()) && fCurrent->InheritsFrom(TH1F::Class())) { skip = true; continue; }

         } while(skip);

         if (fCurrent && fCurrent->InheritsFrom(THStack::Class()))
        	 fStacks.Add(new TObjString(fCurrent->GetName()));

         return (fCurrent!=0);
      }

      virtual Bool_t isfolder()
      {
         return (dynamic_cast<TPad*>(fCurrent)!=0);
      }

      virtual TGo4LevelIter* subiterator()
      {
         TPad* subpad = dynamic_cast<TPad*>(fCurrent);
         return (subpad==0) ? 0 : new TGo4CanvasLevelIter(subpad);
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
         return isfolder() ? TGo4Access::kndFolder : TGo4Access::kndObject;
      }

      virtual const char* GetClassName()
      {
         return fCurrent->ClassName();
      }
};

// ****************************************************************

TGo4CanvasProxy::TGo4CanvasProxy() :
   TGo4Proxy(),
   fCanvas(0),
   fOwner(kFALSE)
{
}

TGo4CanvasProxy::TGo4CanvasProxy(TCanvas* c, Bool_t owner) :
   TGo4Proxy(),
   fCanvas(c),
   fOwner(owner)
{
}

TGo4CanvasProxy::~TGo4CanvasProxy()
{
   if (fOwner) delete fCanvas;
}

Int_t TGo4CanvasProxy::GetObjectKind()
{
   return (fCanvas!=0) ? TGo4Access::kndFolder : TGo4Access::kndNone;
}

const char* TGo4CanvasProxy::GetContainedClassName()
{
   return fCanvas ? fCanvas->ClassName() : 0;
}

void TGo4CanvasProxy::Initialize(TGo4Slot* slot)
{
   TGo4ObjectManager* om = slot->GetOM();
   if (om!=0) om->RegisterObjectWith(fCanvas, slot);
}

void TGo4CanvasProxy::Finalize(TGo4Slot* slot)
{
   TGo4ObjectManager* om = slot->GetOM();
   if (om!=0) om->UnregisterObject(fCanvas, slot);
}

Bool_t TGo4CanvasProxy::RemoveRegisteredObject(TObject* obj)
{
   if (obj==fCanvas) {
      fCanvas = 0;
      fOwner = kFALSE;
   }
   return kFALSE;
}

void TGo4CanvasProxy::WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs)
{
   const char* objname = 0;
   if (fCanvas!=0)
      objname = fCanvas->GetName();

   if (!onlyobjs)
     slot->SetPar("CanvasProxy::CanvasName", objname);

   if ((dir==0) || (fCanvas==0)) return;

   dir->cd();

   fCanvas->Write(objname);
}

void TGo4CanvasProxy::ReadData(TGo4Slot* slot, TDirectory* dir)
{
   const char* objname = slot->GetPar("CanvasProxy::CanvasName");
   if ((objname==0) || (dir==0)) return;

   dir->cd();

   AssignObject(slot, dir->Get(objname), kTRUE);
}

Bool_t TGo4CanvasProxy::IsAcceptObject(TClass* cl)
{
   return (cl!=0) && cl->InheritsFrom(TCanvas::Class());
}

Bool_t TGo4CanvasProxy::AssignObject(TGo4Slot* slot, TObject* obj, Bool_t owner)
{
   Finalize(slot);
   if ((fCanvas!=0) && fOwner) delete fCanvas;

   fCanvas = dynamic_cast<TCanvas*> (obj);
   fOwner = owner && (fCanvas!=0);

   if ((fCanvas==0) && (obj!=0) && owner) delete obj;

   Initialize(slot);

   slot->ForwardEvent(slot, TGo4Slot::evObjAssigned);

   return fCanvas!=0;
}

TObject* TGo4CanvasProxy::GetAssignedObject()
{
   return fCanvas;
}

TGo4Access* TGo4CanvasProxy::ProduceProxy(TCanvas* canv, const char* name)
{
   if (canv==0) return 0;
   if ((name==0) || (*name==0)) return new TGo4ObjectAccess(canv);

   TPad* curpad = canv;
   const char* curname = name;

   while (curpad!=0) {
      const char* slash = strchr(curname,'/');
      UInt_t len = (slash!=0) ? slash - curname : strlen(curname);
      TIter iter(curpad->GetListOfPrimitives());
      TObject* obj = 0;
      while ((obj = iter())!=0)
         if ((strlen(obj->GetName())==len) &&
             (strncmp(obj->GetName(), curname, len)==0)) break;
      if (obj==0) return 0;

//      cout << "Object class = " << obj->ClassName() << endl;

      if (slash==0) return new TGo4ObjectAccess(obj);

      curname = slash+1;

      curpad = dynamic_cast<TPad*>(obj);
   }

   return 0;
}

TGo4LevelIter* TGo4CanvasProxy::ProduceIter(TCanvas* canv)
{
   return new TGo4CanvasLevelIter(canv);
}
