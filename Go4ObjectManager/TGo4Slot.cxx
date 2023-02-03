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

#include "TGo4Slot.h"

#include "TROOT.h"
#include "TClass.h"

#include "TGo4Iter.h"
#include "TGo4ObjectProxy.h"

class TGo4SlotIter : public TGo4LevelIter {
   public:
      TGo4SlotIter() : TGo4LevelIter(), fSlot(nullptr), fIndex(-1) {}

      TGo4SlotIter(const TGo4Slot *slot) : TGo4LevelIter(), fSlot(slot), fIndex(-1) {}

      virtual ~TGo4SlotIter() {}

      Bool_t next() override
         { return fSlot && (++fIndex<fSlot->NumChilds()); }

      Bool_t isfolder() override { return curSlot()->HasSubLevels(); }

      Bool_t isslotsfolder() override { return curSlot()->HasSlotsSubLevels(); }

      TGo4LevelIter* subiterator() override { return curSlot()->MakeLevelIter(); }

      TGo4Slot *getslot() override { return curSlot(); }

      const char *name() override { return curSlot()->GetName();  }

      const char *info() override { return curSlot()->GetInfo(); }

      Int_t sizeinfo() override { return curSlot()->GetSizeInfo(); }

      Int_t GetKind() override { return curSlot()->GetSlotKind(); }

      const char *GetClassName() override { return curSlot()->GetSlotClassName(); }

    protected:
       TGo4Slot *curSlot() const { return fSlot->GetChild(fIndex); }

       const TGo4Slot *fSlot{nullptr};  //!
       Int_t           fIndex{-1};      //!
};

// **********************************************************************************


TGo4Slot::TGo4Slot() :
   TNamed()
{
   SetBit(kStartDelete, kFALSE);
}

TGo4Slot::TGo4Slot(TGo4Slot *parent) :
   TNamed(),
   fParent(parent)
{
   SetBit(kStartDelete, kFALSE);

   if (parent)
     parent->AddChild(this);

   Event(this, evCreate);
}

TGo4Slot::TGo4Slot(TGo4Slot *parent, const char *name, const char *title) :
   TNamed(name, title),
   fParent(parent)
{
   SetBit(kStartDelete, kFALSE);

   if (parent)
     parent->AddChild(this);

   Event(this, evCreate);
}

TGo4Slot::~TGo4Slot()
{
   // sequence of calls here is very important
   // First, we emit message from slot itself to inform all dependent slots
   // that we intend to delete our objects. After that all dependent slots
   // are informed and we can remove object (with clean proxy), delete childs
   // and remove parameters

   if (gDebug>1) Info("~TGo4Slot","%p Starting name = %s", this, GetFullName().Data());

   SetBit(kStartDelete, kTRUE);

   if (gDebug>1) Info("~TGo4Slot","%p CleanProxy()", this);
   CleanProxy();

   if (gDebug>1) Info("~TGo4Slot","%p Event(this, evDelete)", this);
   Event(this, evDelete);

   if (gDebug>1) Info("~TGo4Slot","%p DeleteChilds()", this);
   DeleteChilds();

   if (gDebug>1) Info("~TGo4Slot","%p Detach from parent", this);
   if (fParent) {
      fParent->RemoveChild(this);
      fParent = nullptr;
   }

   if (gDebug>1) Info("~TGo4Slot","%p fPars.Delete()", this);
   fPars.Delete();

   if (fChilds) {
      if (gDebug>1) Info("~TGo4Slot","%p Detach rest children", this);
      for (Int_t n = 0; n <= fChilds->GetLast(); n++) {
         TGo4Slot *child = (TGo4Slot *) fChilds->At(n);
         if (!child) continue;
         child->fParent = nullptr;
         fChilds->Remove(child);
      }

      delete fChilds;
      fChilds = nullptr;
   }

   if (gDebug>1) Info("~TGo4Slot","%p Finish", this);


// *********************************************************************
//  This is new sequence, but also has a problems
//   SetBit(kStartDelete, kTRUE);

//   Info("~TGo4Slot","DeleteChilds() %x", this);
//   DeleteChilds();

//   Info("~TGo4Slot","Event(this, evDelete) %x %s", this, GetFullName().Data());
//   Event(this, evDelete);

//   Info("~TGo4Slot","CleanProxy() %x", this);
//   CleanProxy();

//   Info("~TGo4Slot","Dettach from parent %x", this);
//   if (fParent) {
//      fParent->RemoveChild(this);
//      fParent = nullptr;
//   }

//   Info("~TGo4Slot","fPars.Delete() %x", this);
//   fPars.Delete();




// *********************************************************************
//   This is old sequence. CleanProxy() breaks all dependency and delete objects first
//   before message can be distributed over dependent slots
//   CleanProxy();
//   DeleteChilds();
//   Event(this, evDelete);
//   fPars.Delete();
}

void TGo4Slot::Delete(Option_t *)
{
   if (DoingDelete()) return;

   delete this;
}

Bool_t TGo4Slot::IsParent(const TGo4Slot *slot) const
{
   TGo4Slot *parent = GetParent();
   while (parent) {
      if (parent==slot) return kTRUE;
      parent = parent->GetParent();
   }
   return kFALSE;
}

void TGo4Slot::DeleteChild(const char *name)
{
   TGo4Slot *child = FindChild(name);
   if (!child) return;

   if (!child->DoingDelete())
      delete child;

   if ((NumChilds() == 0) && fChilds) {
      delete fChilds;
      fChilds = nullptr;
   }
}

void TGo4Slot::DeleteChilds(const char *startedwith)
{
   UInt_t len = !startedwith ? 0 : strlen(startedwith);

   for (Int_t n = NumChilds() - 1; n >= 0; n--) {
      TGo4Slot *child = GetChild(n);
      if (!child) continue;

      Bool_t flag = (len == 0) ||
          ((len<strlen(child->GetName())) &&
           (strncmp(child->GetName(), startedwith, len) == 0));

      if (!flag) continue;

      if (!child->DoingDelete())
         delete child;
   }

   if (fChilds && (NumChilds() == 0)) {
      delete fChilds;
      fChilds = nullptr;
   }
}

Int_t TGo4Slot::GetIndexOf(const TGo4Slot *child) const
{
   if (!child) return -1;
   for (int n = 0; n < NumChilds(); n++)
      if (GetChild(n) == child)
         return n;
   return -1;
}


TGo4Slot *TGo4Slot::GetNextChild(const TGo4Slot *child) const
{
   if (!child) return nullptr;
   for (int n = 0; n < NumChilds() - 1; n++)
      if (GetChild(n) == child)
         return GetChild(n + 1);
   return nullptr;
}

TGo4Slot *TGo4Slot::FindChild(const char *name) const
{
   if (!name || !*name) return nullptr;
   Int_t num = NumChilds();
   for (Int_t n = 0; n < num; n++) {
      TGo4Slot *slot = GetChild(n);
      if (strcmp(slot->GetName(), name) == 0)
         return slot;
   }
   return nullptr;
}


TGo4Slot *TGo4Slot::GetNext() const
{
   TGo4Slot *parent = GetParent();
   return !parent ? nullptr : parent->GetNextChild(this);
}

void TGo4Slot::ProduceFullName(TString &name, TGo4Slot *toparent)
{
   if (GetParent() && (GetParent() != toparent)) {
      GetParent()->ProduceFullName(name, toparent);
      if (name.Length() > 0) name += "/";
      name += GetName();
   } else
      name = GetName();
}

TString TGo4Slot::GetFullName(TGo4Slot *toparent)
{
   TString res;
   ProduceFullName(res, toparent);
   return res;
}


TGo4ObjectManager *TGo4Slot::GetOM() const
{
  return GetParent() ? GetParent()->GetOM() : nullptr;
}

void TGo4Slot::CleanProxy()
{
   if (fProxy) {
      fProxy->Finalize(this);
      delete fProxy;
      fProxy = nullptr;
   }
}

void TGo4Slot::SetProxy(TGo4Proxy* cont)
{
    CleanProxy();

    fProxy = cont;

    const char *contclass = fProxy ? fProxy->ClassName() : nullptr;

    SetPar("::ProxyClass", contclass);

    if (fProxy) {
      fProxy->Initialize(this);
      Event(this, evContAssigned);
    }
}

const char *TGo4Slot::GetInfo()
{
   const char *info = nullptr;
   if (fProxy)
     info = fProxy->GetContainedObjectInfo();
   if (!info) info = GetTitle();
   return info;
}

Int_t TGo4Slot::GetSizeInfo()
{
   Int_t sz = -1;
   if (fProxy)
     sz = fProxy->GetObjectSizeInfo();
   return sz;
}

Int_t TGo4Slot::GetSlotKind() const
{
   return fProxy ? fProxy->GetObjectKind() : TGo4Access::kndFolder;
}

const char *TGo4Slot::GetSlotClassName() const
{
   return fProxy ? fProxy->GetContainedClassName() : nullptr;
}

Bool_t TGo4Slot::IsAcceptObject(TClass *cl) const
{
   return fProxy ? fProxy->IsAcceptObject(cl) : kFALSE;
}

Bool_t TGo4Slot::AssignObject(TObject *obj, Bool_t owner)
{
   fAssignCnt++;
   fAssignFlag = kFALSE;
   if (fProxy)
      fAssignFlag = fProxy->AssignObject(this, obj, owner);
   else if (owner)
      delete obj;

   return (fAssignFlag == (Int_t) kTRUE);
}

TObject *TGo4Slot::GetAssignedObject()
{
   return fProxy ? fProxy->GetAssignedObject() : nullptr;
}

void TGo4Slot::Update(Bool_t strong)
{
   if (fProxy)
     fProxy->Update(this, strong);

   for (int n = 0; n < NumChilds(); n++)
      GetChild(n)->Update(strong);
}

Bool_t TGo4Slot::HasSubLevels() const
{
   if (fProxy && fProxy->Use()) return fProxy->HasSublevels();

   return HasSlotsSubLevels();
}

Bool_t TGo4Slot::HasSlotsSubLevels() const
{
   return NumChilds() > 0;
}

TGo4LevelIter* TGo4Slot::MakeLevelIter() const
{
   TGo4LevelIter* res = nullptr;

   if (fProxy && fProxy->Use())
     res = fProxy->MakeIter();

   if (!res && (NumChilds() > 0))
      res = new TGo4SlotIter(this);

   return res;
}

std::unique_ptr<TGo4Access> TGo4Slot::ProvideSlotAccess(const char *name)
{
   if (fProxy && fProxy->Use())
      return fProxy->ProvideAccess(name);

   if (!name || !*name)
      return std::make_unique<TGo4ObjectAccess>(this);

   const char *subname = nullptr;

   TGo4Slot *subslot = DefineSubSlot(name, subname);

   return !subslot ? nullptr : subslot->ProvideSlotAccess(subname);
}

void TGo4Slot::SaveData(TDirectory *dir, Bool_t onlyobjs)
{
   if (fProxy)
     fProxy->WriteData(this, dir, onlyobjs);
}

void TGo4Slot::ReadData(TDirectory *dir)
{
   CleanProxy();

   const char *contclass = GetPar("::ProxyClass");
   TClass *cl = !contclass ? nullptr : gROOT->GetClass(contclass);
   if (!cl) return;

   TGo4Proxy* cont = (TGo4Proxy*) cl->New();

   cont->ReadData(this, dir);

   SetProxy(cont);
}

TGo4Slot *TGo4Slot::DefineSubSlot(const char *name, const char *&subname) const
{
   Int_t len = 0;

   const char *spos = strchr(name,'/');

   if (!spos) { len = strlen(name); subname = nullptr; }
         else { len = spos-name; subname = spos+1; }
   UInt_t ulen = (UInt_t) len;

   Int_t num = NumChilds();
   for (int n = 0; n < num; n++) {
      TGo4Slot *slot = GetChild(n);
      const char *slotname = slot->GetName();
      if ((strlen(slotname)==ulen) && (strncmp(slotname, name, len) == 0)) return slot;
   }

   return nullptr;
}

TGo4Slot *TGo4Slot::GetSlot(const char *name, Bool_t force)
{
   if (!name || !*name) return this;

   const char *subname = nullptr;

   TGo4Slot *subslot = DefineSubSlot(name, subname);

   if (!subslot && force) {
      TString newname;
      if (!subname) newname = name;
               else newname.Append(name, subname-name-1);
      subslot = new TGo4Slot(this, newname.Data(), "folder");
   }

   return !subslot ? nullptr : subslot->GetSlot(subname, force);
}

TGo4Slot *TGo4Slot::FindSlot(const char *fullpath, const char** subname)
{
   // exclude current dir and process parent dir
   while (fullpath && (strlen(fullpath) > 2)) {
      // ignore current dir
      if (strncmp(fullpath, "./", 2) == 0) { fullpath += 2; continue; }
      // process parent dir

      if ((strncmp(fullpath, "../", 3) == 0) && GetParent())
         return GetParent()->FindSlot(fullpath + 3, subname);

      break;
   }

   TGo4Slot *slot = GetSlot(fullpath);
   if (slot) {
      if (subname) *subname = nullptr;
      return slot;
   }

   const char *curname = fullpath;
   TGo4Slot *curslot = this;

   while (curslot) {
      const char *nextname = nullptr;
      TGo4Slot *nextslot = curslot->DefineSubSlot(curname, nextname);
      if (!nextslot) break;
      curslot = nextslot;
      curname = nextname;
   }

   if (subname) *subname = curname;
   return curslot;
}


Bool_t TGo4Slot::ShiftSlotBefore(TGo4Slot *slot, TGo4Slot *before)
{
   if (!fChilds) return kFALSE;
   Int_t indx1 = !before ? -1 : fChilds->IndexOf(before);
   Int_t indx2 = !slot ? -1 : fChilds->IndexOf(slot);
   if ((indx1 < 0) || (indx2 < 0) || (indx1 > indx2)) return kFALSE;
   if (indx1 == indx2) return kTRUE;

   for (int n = indx2; n > indx1; n--)
      (*fChilds)[n] = (*fChilds)[n - 1];

   (*fChilds)[indx1] = slot;

   return kTRUE;
}

Bool_t TGo4Slot::ShiftSlotAfter(TGo4Slot *slot, TGo4Slot *after)
{
   if (!fChilds) return kFALSE;
   Int_t indx1 = !slot ? -1 : fChilds->IndexOf(slot);
   Int_t indx2 = !after ? -1 : fChilds->IndexOf(after);
   if ((indx1<0) || (indx2<0) || (indx1>indx2)) return kFALSE;

   if (indx1==indx2) return kTRUE;

   for (int n=indx1;n<indx2;n++)
     (*fChilds)[n] = (*fChilds)[n+1];
   (*fChilds)[indx2] = slot;

   return kTRUE;
}

void TGo4Slot::AddChild(TGo4Slot *child)
{
   if (!child) return;
   if (!fChilds) fChilds = new TObjArray;
   fChilds->Add(child);
}

void TGo4Slot::RemoveChild(TGo4Slot *child)
{
    if (!child || !fChilds) return;
    fChilds->Remove(child);
    fChilds->Compress();
    if (fChilds->GetLast()<0) {
       delete fChilds;
       fChilds = nullptr;
    }
}

void TGo4Slot::Event(TGo4Slot *source, Int_t id, void *param)
{
   Bool_t doforward = kTRUE;

   if (fProxy)
      doforward = fProxy->ProcessEvent(this, source, id, param);

   if (doforward) ForwardEvent(source, id, param);
}

void TGo4Slot::ForwardEvent(TGo4Slot *source, Int_t id, void *param)
{
  if (GetParent())
     GetParent()->Event(source, id, param);
}

void TGo4Slot::RecursiveRemove(TObject *obj)
{
   if (fProxy)
      if (fProxy->RemoveRegisteredObject(obj))
        delete this;
}

void TGo4Slot::Print(Option_t *option) const
{
   TGo4Iter iter((TGo4Slot *)this);
   while (iter.next()) {
      printf("%*c%s%s - %s\n", (iter.level()+1)*2, ' ', iter.getname(), (iter.isfolder() ? "/" : ""),  iter.getinfo());
   }
}

void TGo4Slot::SetPar(const char *name, const char *value)
{
   if (!name || !*name) return;
   if (!value) { RemovePar(name); return; }

   TNamed *par = (TNamed*) fPars.FindObject(name);
   if (par)
     par->SetTitle(value);
   else
     fPars.Add(new TNamed(name,value));
}

const char *TGo4Slot::GetPar(const char *name) const
{
   if (!name || !*name)
      return nullptr;
   TNamed *par = (TNamed*) fPars.FindObject(name);
   return par ? par->GetTitle() : nullptr;
}

void TGo4Slot::RemovePar(const char *name)
{
   if (!name || !*name) return;
   TNamed *par = (TNamed*) fPars.FindObject(name);
   if (par) {
      fPars.Remove(par);
      fPars.Compress();
      delete par;
   }
}

void TGo4Slot::SetIntPar(const char *name, Int_t value)
{
   TString buf;
   buf.Form("%d",value);
   SetPar(name, buf.Data());
}

Bool_t TGo4Slot::GetIntPar(const char *name, Int_t& value) const
{
   const char *strvalue = GetPar(name);
   if (!strvalue) return kFALSE;
   value = atoi(strvalue);
   return kTRUE;
}

void TGo4Slot::PrintPars(Int_t level)
{
   for (int n = 0; n <= fPars.GetLast(); n++) {
      TNamed *par = (TNamed*) fPars.At(n);
      if (par)
        printf("%*c%s = %s\n", level, ' ', par->GetName(), par->GetTitle());
   }
}

const char *TGo4Slot::FindFolderSeparator(const char *name)
{
   return !name ? nullptr : strrchr(name,'/');
}

void TGo4Slot::ProduceFolderAndName(const char *fullname, TString &foldername, TString &objectname)
{
   const char *rslash = FindFolderSeparator(fullname);
   foldername = "";

   if (!rslash) {
      objectname = fullname;
   } else {
      foldername.Append(fullname, rslash-fullname);
      objectname = (rslash+1);
   }
}
