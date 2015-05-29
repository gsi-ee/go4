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

#include "TGo4Slot.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "TROOT.h"
#include "TClass.h"

#include "TGo4Iter.h"
#include "TGo4ObjectProxy.h"

class TGo4SlotIter : public TGo4LevelIter {
   public:
      TGo4SlotIter() : TGo4LevelIter(), fSlot(0), fIndex(-1) {}

      TGo4SlotIter(const TGo4Slot* slot) : TGo4LevelIter(), fSlot(slot), fIndex(-1) {}

      virtual ~TGo4SlotIter() {}

      virtual Bool_t next()
         { return (fSlot!=0) && (++fIndex<fSlot->NumChilds()); }

      virtual Bool_t isfolder() { return curSlot()->HasSubLevels(); }

      virtual Bool_t isslotsfolder() { return curSlot()->HasSlotsSubLevels(); }

      virtual TGo4LevelIter* subiterator() { return curSlot()->MakeLevelIter(); }

      virtual TGo4Slot* getslot() { return curSlot(); }

      virtual const char* name() { return curSlot()->GetName();  }

      virtual const char* info() { return curSlot()->GetInfo(); }

      virtual Int_t sizeinfo() { return curSlot()->GetSizeInfo(); }

      virtual Int_t GetKind() { return curSlot()->GetSlotKind(); }

      virtual const char* GetClassName() { return curSlot()->GetSlotClassName(); }

    protected:
       TGo4Slot* curSlot() const { return fSlot->GetChild(fIndex); }

       const TGo4Slot*  fSlot;  //!
       Int_t      fIndex;       //!
};

// **********************************************************************************


TGo4Slot::TGo4Slot() :
   TNamed(),
   fParent(0),
   fChilds(0),
   fPars(),
   fProxy(0),
   fAssignFlag(-1),
   fAssignCnt(0)
{
   SetBit(kStartDelete, kFALSE);
}

TGo4Slot::TGo4Slot(TGo4Slot* parent) :
   TNamed(),
   fParent(parent),
   fChilds(0),
   fPars(),
   fProxy(0),
   fAssignFlag(-1),
   fAssignCnt(0)
{
   SetBit(kStartDelete, kFALSE);

   if (parent!=0)
     parent->AddChild(this);

   Event(this, evCreate);
}

TGo4Slot::TGo4Slot(TGo4Slot* parent, const char* name, const char* title) :
   TNamed(name, title),
   fParent(parent),
   fChilds(0),
   fPars(),
   fProxy(0),
   fAssignFlag(-1),
   fAssignCnt(0)
{
   SetBit(kStartDelete, kFALSE);

   if (parent!=0)
     parent->AddChild(this);

   Event(this, evCreate);
}

TGo4Slot::~TGo4Slot()
{
   // sequence of calls here is very important
   // First, we emit message from slot itself to inform all dependent slots
   // that we intend to delete our objects. After that all depenedent slots
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
   if (fParent!=0) {
      fParent->RemoveChild(this);
      fParent = 0;
   }

   if (gDebug>1) Info("~TGo4Slot","%p fPars.Delete()", this);
   fPars.Delete();

   if (fChilds!=0) {
      if (gDebug>1) Info("~TGo4Slot","%p Detach rest children", this);
      for (Int_t n=0;n<=fChilds->GetLast();n++) {
         TGo4Slot* child = (TGo4Slot*) fChilds->At(n);
         if (child==0) continue;
         child->fParent = 0;
         fChilds->Remove(child);
      }

      delete fChilds;
      fChilds = 0;
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
//   if (fParent!=0) {
//      fParent->RemoveChild(this);
//      fParent = 0;
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

Bool_t TGo4Slot::IsParent(const TGo4Slot* slot) const
{
   TGo4Slot* parent = GetParent();
   while (parent!=0) {
      if (parent==slot) return kTRUE;
      parent = parent->GetParent();
   }
   return kFALSE;
}

void TGo4Slot::DeleteChild(const char* name)
{
   TGo4Slot* child = FindChild(name);
   if (child==0) return;

   if (!child->DoingDelete())
      delete child;

   if ((NumChilds()==0) && (fChilds!=0)) {
      delete fChilds;
      fChilds = 0;
   }
}

void TGo4Slot::DeleteChilds(const char* startedwith)
{
   UInt_t len = startedwith==0 ? 0 : strlen(startedwith);

   for(Int_t n = NumChilds()-1; n>=0; n--) {
      TGo4Slot* child = GetChild(n);
      if (child==0) continue;

      Bool_t flag = (len==0) ||
          ((len<strlen(child->GetName())) &&
           (strncmp(child->GetName(), startedwith, len)==0));

      if (!flag) continue;

      if (!child->DoingDelete())
         delete child;
   }

   if ((fChilds!=0) && (NumChilds()==0)) {
      delete fChilds;
      fChilds = 0;
   }
}

Int_t TGo4Slot::GetIndexOf(TGo4Slot* child)
{
   if (child==0) return -1;
   for(int n=0;n<NumChilds();n++)
     if (GetChild(n)==child) return n;
   return -1;
}


TGo4Slot* TGo4Slot::GetNextChild(TGo4Slot* child)
{
   if (child==0) return 0;
   for(int n=0;n<NumChilds()-1;n++)
     if (GetChild(n)==child) return GetChild(n+1);
   return 0;
}

TGo4Slot* TGo4Slot::FindChild(const char* name)
{
   if ((name==0) || (*name==0)) return 0;
   Int_t num = NumChilds();
   for(Int_t n=0;n<num;n++) {
      TGo4Slot* slot = GetChild(n);
      if (strcmp(slot->GetName(), name)==0) return slot;
   }
   return 0;
}


TGo4Slot* TGo4Slot::GetNext()
{
   TGo4Slot* parent = GetParent();
   return (parent==0) ? 0 : parent->GetNextChild(this);
}

void TGo4Slot::ProduceFullName(TString& name, TGo4Slot* toparent)
{
   if ((GetParent()!=0) && (GetParent()!=toparent)) {
      GetParent()->ProduceFullName(name, toparent);
      if (name.Length()>0) name += "/";
      name+=GetName();
   } else
      name = GetName();
}

TString TGo4Slot::GetFullName(TGo4Slot* toparent)
{
   TString res;
   ProduceFullName(res, toparent);
   return res;
}


TGo4ObjectManager* TGo4Slot::GetOM() const
{
  return GetParent() ? GetParent()->GetOM() : 0;
}

void TGo4Slot::CleanProxy()
{
    if (fProxy!=0) {
//       Info("CleanProxy","Proxy %s", fProxy->ClassName());
       fProxy->Finalize(this);
       delete fProxy;
       fProxy = 0;
    }
}

void TGo4Slot::SetProxy(TGo4Proxy* cont)
{
    CleanProxy();

    fProxy = cont;

    const char* contclass = (fProxy!=0) ? fProxy->ClassName() : 0;

    SetPar("::ProxyClass", contclass);

    if (fProxy!=0) {
      fProxy->Initialize(this);
      Event(this, evContAssigned);
    }
}

const char* TGo4Slot::GetInfo()
{
   const char* info = 0;
   if (fProxy!=0)
     info = fProxy->GetContainedObjectInfo();
   if (info==0) info = GetTitle();
   return info;
}

Int_t TGo4Slot::GetSizeInfo()
{
   Int_t sz = -1;
   if (fProxy!=0)
     sz = fProxy->GetObjectSizeInfo();
   return sz;
}

Int_t TGo4Slot::GetSlotKind() const
{
   return (fProxy!=0) ? fProxy->GetObjectKind() : TGo4Access::kndFolder;
}

const char* TGo4Slot::GetSlotClassName() const
{
   return (fProxy!=0) ? fProxy->GetContainedClassName() : 0;
}

Bool_t TGo4Slot::IsAcceptObject(TClass* cl)
{
   return fProxy!=0 ? fProxy->IsAcceptObject(cl) : kFALSE;
}

Bool_t TGo4Slot::AssignObject(TObject* obj, Bool_t owner)
{
   fAssignCnt++;
   fAssignFlag = kFALSE;
   if (fProxy!=0)
      fAssignFlag = fProxy->AssignObject(this, obj, owner);
   else
      if (owner) delete obj;

   return (fAssignFlag == (Int_t) kTRUE);
}

TObject* TGo4Slot::GetAssignedObject()
{
   if (fProxy==0) return 0;
   return fProxy->GetAssignedObject();
}

void TGo4Slot::Update(Bool_t strong)
{
   if (fProxy!=0)
     fProxy->Update(this, strong);

   for(int n=0;n<NumChilds();n++)
     GetChild(n)->Update(strong);
}


Bool_t TGo4Slot::HasSubLevels() const
{
   if ((fProxy!=0) && fProxy->Use()) return fProxy->HasSublevels();

   return HasSlotsSubLevels();
}

Bool_t TGo4Slot::HasSlotsSubLevels() const
{
   return NumChilds()>0;
}

TGo4LevelIter* TGo4Slot::MakeLevelIter() const
{
   TGo4LevelIter* res = 0;

   if ((fProxy!=0) && fProxy->Use())
     res = fProxy->MakeIter();

   if ((res==0) && (NumChilds() > 0)) res = new TGo4SlotIter(this);

   return res;
}

TGo4Access* TGo4Slot::ProvideSlotAccess(const char* name)
{
//   std::cout << " TGo4Slot::GetSlotProxy " << name << "  slot = " << GetName()
//        << " cont = " << (fProxy ? fProxy->ClassName() : "null") << std::endl;

   if ((fProxy!=0) && fProxy->Use())
      return fProxy->ProvideAccess(name);

   if ((name==0) || (*name==0)) return new TGo4ObjectAccess(this);

   const char* subname = 0;

   TGo4Slot* subslot = DefineSubSlot(name, subname);

   return (subslot==0) ? 0 : subslot->ProvideSlotAccess(subname);
}

void TGo4Slot::SaveData(TDirectory* dir, Bool_t onlyobjs)
{
   if (fProxy!=0)
     fProxy->WriteData(this, dir, onlyobjs);
}

void TGo4Slot::ReadData(TDirectory* dir)
{
   CleanProxy();

   const char* contclass = GetPar("::ProxyClass");
   TClass* cl = (contclass==0) ? 0 : gROOT->GetClass(contclass);
   if (cl==0) return;

   TGo4Proxy* cont = (TGo4Proxy*) cl->New();

   cont->ReadData(this, dir);

   SetProxy(cont);
}

TGo4Slot* TGo4Slot::DefineSubSlot(const char* name, const char* &subname) const
{
   Int_t len = 0;

   const char* spos = strchr(name,'/');

   if (spos==0) { len = strlen(name); subname=0; }
           else { len = spos-name; subname=spos+1; }
   UInt_t ulen = (UInt_t) len;

   Int_t num = NumChilds();
   for(int n=0;n<num;n++) {
      TGo4Slot* slot = GetChild(n);
      const char* slotname = slot->GetName();
      if ((strlen(slotname)==ulen) && (strncmp(slotname, name, len)==0)) return slot;
   }

   return 0;
}

TGo4Slot* TGo4Slot::GetSlot(const char* name, Bool_t force)
{
   if ((name==0) || (*name==0)) return this;

   const char* subname = 0;

   TGo4Slot* subslot = DefineSubSlot(name, subname);

   if ((subslot==0) && force) {
      TString newname;
      if (subname==0) newname = name;
                 else newname.Append(name, subname-name-1);
      subslot = new TGo4Slot(this, newname.Data(), "folder");
   }

   return subslot==0 ? 0 : subslot->GetSlot(subname, force);
}

TGo4Slot* TGo4Slot::FindSlot(const char* fullpath, const char** subname)
{
   // exclude current dir and process parent dir
   while ((fullpath!=0) && (strlen(fullpath)>2)) {
      // ignore current dir
      if (strncmp(fullpath, "./", 2) == 0) { fullpath += 2; continue; }
      // process parent dir

      if ((strncmp(fullpath, "../", 3) == 0) && GetParent())
         return GetParent()->FindSlot(fullpath + 3, subname);

      break;
   }

   TGo4Slot* slot = GetSlot(fullpath);
   if (slot!=0) {
      if (subname!=0) *subname = 0;
      return slot;
   }

   const char* curname = fullpath;
   TGo4Slot* curslot = this;

   while (curslot!=0) {
      const char* nextname = 0;
      TGo4Slot* nextslot = curslot->DefineSubSlot(curname, nextname);
      if (nextslot==0) break;
      curslot = nextslot;
      curname = nextname;
   }

   if (subname!=0) *subname = curname;
   return curslot;
}


Bool_t TGo4Slot::ShiftSlotBefore(TGo4Slot* slot, TGo4Slot* before)
{
   if (fChilds==0) return kFALSE;
   Int_t indx1 = (before==0) ? -1 : fChilds->IndexOf(before);
   Int_t indx2 = (slot==0) ? -1 : fChilds->IndexOf(slot);
   if ((indx1<0) || (indx2<0) || (indx1>indx2)) return kFALSE;
   if (indx1==indx2) return kTRUE;

   for (int n=indx2;n>indx1;n--)
     (*fChilds)[n] = (*fChilds)[n-1];

   (*fChilds)[indx1] = slot;

   return kTRUE;
}

Bool_t TGo4Slot::ShiftSlotAfter(TGo4Slot* slot, TGo4Slot* after)
{
   if (fChilds==0) return kFALSE;
   Int_t indx1 = (slot==0) ? -1 : fChilds->IndexOf(slot);
   Int_t indx2 = (after==0) ? -1 : fChilds->IndexOf(after);
   if ((indx1<0) || (indx2<0) || (indx1>indx2)) return kFALSE;

   if (indx1==indx2) return kTRUE;

   for (int n=indx1;n<indx2;n++)
     (*fChilds)[n] = (*fChilds)[n+1];
   (*fChilds)[indx2] = slot;

   return kTRUE;
}

void TGo4Slot::AddChild(TGo4Slot* child)
{
   if (child==0) return;
   if (fChilds==0) fChilds = new TObjArray;
   fChilds->Add(child);
}

void TGo4Slot::RemoveChild(TGo4Slot* child)
{
    if ((child==0) || (fChilds==0)) return;
    fChilds->Remove(child);
    fChilds->Compress();
    if (fChilds->GetLast()<0) {
       delete fChilds;
       fChilds = 0;
    }
}

void TGo4Slot::Event(TGo4Slot* source, Int_t id, void* param)
{
   Bool_t doforward = kTRUE;

   if (fProxy!=0)
     doforward = fProxy->ProcessEvent(this, source, id, param);

   if (doforward) ForwardEvent(source, id, param);
}

void TGo4Slot::ForwardEvent(TGo4Slot* source, Int_t id, void* param)
{
//  std::cout << "ForwardEvent " << id <<" from " << GetName() << " to "
//       << ((GetParent()!=0) ? GetParent()->GetName() : "null") << std::endl;

  if (GetParent()!=0)
     GetParent()->Event(source, id, param);
}

void TGo4Slot::RecursiveRemove(TObject* obj)
{
   if (fProxy!=0)
      if (fProxy->RemoveRegisteredObject(obj))
        delete this;
}

void TGo4Slot::Print(Option_t* option) const
{
   TGo4Iter iter((TGo4Slot*) this);
   while (iter.next()) {
      printf("%*c%s%s - %s\n", (iter.level()+1)*2, ' ', iter.getname(), (iter.isfolder() ? "/" : ""),  iter.getinfo());
   }
}

void TGo4Slot::SetPar(const char* name, const char* value)
{
   if ((name==0) || (*name==0)) return;
   if (value==0) { RemovePar(name); return; }

   TNamed* par = (TNamed*) fPars.FindObject(name);
   if (par!=0)
     par->SetTitle(value);
   else
     fPars.Add(new TNamed(name,value));
}

const char* TGo4Slot::GetPar(const char* name) const
{
   if ((name==0) || (*name==0)) return 0;
   TNamed* par = (TNamed*) fPars.FindObject(name);
   return (par!=0) ? par->GetTitle() : 0;
}

void TGo4Slot::RemovePar(const char* name)
{
   if ((name==0) || (*name==0)) return;
   TNamed* par = (TNamed*) fPars.FindObject(name);
   if (par!=0) {
      fPars.Remove(par);
      fPars.Compress();
      delete par;
   }
}

void TGo4Slot::SetIntPar(const char* name, Int_t value)
{
   TString buf;
   buf.Form("%d",value);
   SetPar(name, buf.Data());
}

Bool_t TGo4Slot::GetIntPar(const char* name, Int_t& value)
{
   const char* strvalue = GetPar(name);
   if (strvalue==0) return kFALSE;
   value = atoi(strvalue);
   return kTRUE;
}

void TGo4Slot::PrintPars(Int_t level)
{
   for (int n=0;n<=fPars.GetLast();n++) {
      TNamed* par = (TNamed*) fPars.At(n);
      if (par!=0)
        printf("%*c%s = %s\n", level, ' ', par->GetName(), par->GetTitle());
   }
}

const char* TGo4Slot::FindFolderSeparator(const char* name)
{
   return name==0 ? 0 : strrchr(name,'/');
}

void TGo4Slot::ProduceFolderAndName(const char* fullname, TString& foldername, TString& objectname)
{
   const char* rslash = FindFolderSeparator(fullname);
   foldername = "";

   if (rslash==0) {
      objectname = fullname;
   } else {
      foldername.Append(fullname, rslash-fullname);
      objectname = (rslash+1);
   }
}
