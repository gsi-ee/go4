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

#include "TGo4DirProxy.h"

#include "TKey.h"
#include "TROOT.h"
#include "TH1.h"
#include "TTree.h"
#include "TFile.h"
#include "TCanvas.h"
#include "THStack.h"

#include "TGo4ObjectProxy.h"
#include "TGo4TreeProxy.h"
#include "TGo4CanvasProxy.h"
#include "TGo4HStackProxy.h"
#include "TGo4Slot.h"

class TGo4KeyAccess : public TGo4Access {
   public:
      TGo4KeyAccess(TDirectory* dir, TKey* key) : TGo4Access(), fDir(dir), fKey(key) {}

      Bool_t CanGetObject() const override
        { return kTRUE; }

      Bool_t GetObject(TObject* &obj, Bool_t &owner) const override
      {
         TClass* cl = gROOT->GetClass(fKey->GetClassName());
         if (cl && !cl->IsLoaded()) {
            obj = nullptr;
            owner = kFALSE;
            return kFALSE;
         }

         owner = kTRUE;
         fDir->cd();
         obj = fKey->ReadObj();
         if (!obj) return kFALSE;

         if (obj->InheritsFrom(TH1::Class()) || obj->InheritsFrom(TTree::Class())) owner = kFALSE; else
         if (obj->InheritsFrom(TCanvas::Class())) { fDir->Add(obj); owner = kFALSE; }
         return kTRUE;
      }

      const char* GetObjectName() const override
        { return fKey->GetName(); }

      const char* GetObjectClassName() const override
        { return fKey->GetClassName(); }

   private:
      TDirectory*  fDir{nullptr};     //!
      TKey*        fKey{nullptr};     //!
};

// ************************************************************************

class TGo4DirLevelIter : public TGo4LevelIter {
   protected:
      TDirectory*    fDir{nullptr};        //!
      Bool_t         fReadRight{kFALSE};   //!
      TIterator*     fIter{nullptr};       //!
      Bool_t         fIsKeyIter{kFALSE};   //!
      TObject*       fCurrent{nullptr};    //!
      TString        fNameBuf;             //!

   public:
      TGo4DirLevelIter(TDirectory* dir, Bool_t readright) :
         TGo4LevelIter(),
         fDir(dir),
         fReadRight(readright),
         fIter(nullptr),
         fIsKeyIter(kFALSE),
         fCurrent(nullptr),
         fNameBuf()
      {
         fIter = fDir->GetListOfKeys()->MakeIterator();
         fIsKeyIter = kTRUE;
      }

      virtual ~TGo4DirLevelIter()
      {
         if (fIter) delete fIter;
      }

      Bool_t next() override
      {
         Bool_t donext = kTRUE;

         while (donext) {

            fCurrent = fIter->Next();

            if (!fCurrent) {
              if(fIsKeyIter) {
                delete fIter;
                fIter = fDir->GetList()->MakeIterator();
                fIsKeyIter = kFALSE;
                continue;
              } else
                break;
            }
            donext = kFALSE;

            if (!fIsKeyIter) {
               TKey* key = fDir->FindKey(fCurrent->GetName());
               if (key)
                 if (strcmp(fCurrent->ClassName(), key->GetClassName())==0) donext = kTRUE;
            }
         }
         return fCurrent!=0;
      }

      Bool_t IsContainerClass(TClass* cl)
      {
         if (!cl) return kFALSE;
         return cl->InheritsFrom(TDirectory::Class()) ||
                cl->InheritsFrom(TTree::Class()) ||
                cl->InheritsFrom(TCanvas::Class()) ||
                cl->InheritsFrom(THStack::Class());
      }

      Bool_t isfolder() override
      {
         TClass* cl = nullptr;
         if (fIsKeyIter) {
            TKey* key = (TKey*) fCurrent;
            TObject* obj = fDir->FindObject(key->GetName());
            if (obj) cl = obj->IsA();
            // if (fReadRight)
            //  cl = TGo4Proxy::GetClass(key->GetClassName());
         } else {
            cl = fCurrent->IsA();
         }

         return IsContainerClass(cl);
      }

      TGo4LevelIter* subiterator() override
      {
         TObject* obj = fIsKeyIter ? fDir->Get(fCurrent->GetName()) : fCurrent;

         if (!obj) return nullptr;

         if (obj->InheritsFrom(TTree::Class()))
            return TGo4TreeProxy::ProduceIter((TTree*)obj);

         if (obj->InheritsFrom(TCanvas::Class()))
            return TGo4CanvasProxy::ProduceIter((TCanvas*)obj);

         if (obj->InheritsFrom(THStack::Class()))
            return TGo4HStackProxy::ProduceIter((THStack*)obj);

         TDirectory* subdir = dynamic_cast<TDirectory*> (obj);
         return !subdir ? nullptr : new TGo4DirLevelIter(subdir, fReadRight);
      }

      const char* name() override
      {
         if (!fIsKeyIter) return fCurrent->GetName();
         TKey* key = (TKey*) fCurrent;
         // if (isfolder() || (key->GetCycle()==1)) return key->GetName();
         fNameBuf.Form("%s;%d",key->GetName(),(int) key->GetCycle());
         return fNameBuf.Data();
      }

      const char* info() override
      {
         return fCurrent->GetTitle();
      }

      Int_t sizeinfo() override
      {
         TClass* cl = nullptr;
         Int_t sz = 0;
         if (fIsKeyIter) {
            TKey* key = (TKey*) fCurrent;
            cl = TGo4Proxy::GetClass(key->GetClassName());
            sz = key->GetNbytes();
            TObject* obj = fDir->FindObject(key->GetName());
            if (obj) sz = TGo4ObjectProxy::DefineObjectSize(obj);
         } else {
            cl = fCurrent->IsA();
            sz = TGo4ObjectProxy::DefineObjectSize(fCurrent);
         }
         bool isdir = cl && cl->InheritsFrom(TDirectory::Class());
         return isdir ? 0 : sz;
      }

      Int_t GetKind() override
      {
         if (isfolder()) return TGo4Access::kndFolder;

         if (fIsKeyIter && fReadRight) {
            TKey* key = (TKey*) fCurrent;
            TClass* cl = TGo4Proxy::GetClass(key->GetClassName());
            if (!cl) cl = gROOT->GetClass(key->GetClassName(), kTRUE, kTRUE);
            if (IsContainerClass(cl)) return TGo4Access::kndMoreFolder;
         }

         return TGo4Access::kndObject;
      }

      const char* GetClassName() override
      {
          return fIsKeyIter ? ((TKey*) fCurrent)->GetClassName() : fCurrent->ClassName();
      }
};

//*************************************************************************

TGo4DirProxy::TGo4DirProxy() :
   TGo4Proxy()
{
}

TGo4DirProxy::TGo4DirProxy(TDirectory* dir, Bool_t readright, Bool_t owner) :
   TGo4Proxy()
{
   SetDir(dir, readright, owner);
}

TGo4DirProxy::~TGo4DirProxy()
{
   ClearDir();
}

void TGo4DirProxy::SetDir(TDirectory* dir, Bool_t readright, Bool_t owner)
{
   ClearDir();
   fDir = dir;
   fReadRight = readright;
   fOwner = owner;
}

void TGo4DirProxy::ClearDir()
{
   if (fOwner && fDir) delete fDir;
   fDir = nullptr;
   fOwner = kFALSE;
}

Int_t TGo4DirProxy::GetObjectKind()
{
   return fDir ? TGo4Access::kndFolder : TGo4Access::kndNone;
}

const char* TGo4DirProxy::GetContainedClassName()
{
   return fDir ? fDir->ClassName() : nullptr;
}

const char* TGo4DirProxy::GetContainedObjectInfo()
{
   return fDir ? fDir->GetTitle() : nullptr;
}

Int_t TGo4DirProxy::GetObjectSizeInfo()
{
    TFile* f = dynamic_cast<TFile*> (fDir);
    return !f ? TGo4Proxy::GetObjectSizeInfo() : f->GetSize();
}

TGo4Access* TGo4DirProxy::CreateAccess(TDirectory* dir, Bool_t readright, const char* name, TGo4Slot* browser_slot)
{
   if (!dir) return nullptr;

   if (!name || (*name == 0)) return new TGo4ObjectAccess(dir);

   TDirectory* curdir = dir;
   const char* curname = name;

   while (curdir && curname) {
      const char* slash = strchr(curname,'/');
      TString partname;

      if (!slash) {
         partname = curname;
         curname = nullptr;
      } else {
         partname.Append(curname, slash - curname);
         curname = slash+1;
         if (*curname == 0) curname = nullptr; // if last symbol is slash than finish searching
      }

      char namebuf[10000];
      Short_t cyclebuf(9999);
      TDirectory::DecodeNameCycle(partname.Data(), namebuf, cyclebuf, sizeof(namebuf));

      TObject* obj = curdir->GetList()->FindObject(namebuf);
      if (!obj) {
         TKey* key = curdir->GetKey(namebuf, cyclebuf);
         if (!key) return nullptr;
         if (!readright) return new TGo4KeyAccess(curdir, key);
         curdir->cd();
         obj = curdir->Get(partname.Data());
         if (obj && obj->InheritsFrom(TCanvas::Class())) curdir->Add(obj);
         if (obj && browser_slot)
            browser_slot->ForwardEvent(browser_slot, TGo4Slot::evObjAssigned);
      }

      if (!obj) return nullptr;

      TTree* tr = dynamic_cast<TTree*> (obj);
      if (tr)
         return TGo4TreeProxy::CreateAccess(tr, curname);

      TCanvas* canv = dynamic_cast<TCanvas*> (obj);
      if (canv)
         return TGo4CanvasProxy::CreateAccess(canv, curname);

      THStack* hs = dynamic_cast<THStack*> (obj);
      if (hs)
         return TGo4HStackProxy::CreateAccess(hs, curname);

      curdir = dynamic_cast<TDirectory*>(obj);

      if (!curdir)
         return new TGo4ObjectAccess(obj);
   }
   return nullptr;
}

TGo4LevelIter* TGo4DirProxy::ProduceIter(TDirectory* dir, Bool_t readright)
{
   return new TGo4DirLevelIter(dir, readright);
}

void TGo4DirProxy::WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs)
{
   if (!onlyobjs) {
      const char* filename = nullptr;
      if ((fDir!=0) && fDir->InheritsFrom(TFile::Class()))
         filename = fDir->GetName();

     slot->SetPar("DirProxy::FileName", filename);
     slot->SetPar("DirProxy::gROOT", (fDir==gROOT) ? "true" : nullptr);
   }
}

void TGo4DirProxy::ReadData(TGo4Slot* slot, TDirectory* dir)
{
   ClearDir();

   const char* filename = slot->GetPar("DirProxy::FileName");
   const char* groot = slot->GetPar("DirProxy::gROOT");

   if (filename) {
     TFile* f = TFile::Open(filename);
     if (f) SetDir(f, kTRUE, kTRUE);
   } else if (groot)
     SetDir(gROOT, kFALSE, kFALSE);
}

Bool_t TGo4DirProxy::UpdateObjectInFile(const char* filepath, TObject* obj)
{
   if (!filepath || !fDir) return kFALSE;

   TFile* f = dynamic_cast<TFile*> (fDir);
   if (!f) return kFALSE;

   if (f->ReOpen("UPDATE")<0) return kFALSE;

   TString foldername, objname;
   TGo4Slot::ProduceFolderAndName(filepath, foldername, objname);

   TDirectory* objdir = f;

   if (foldername.Length() > 0)
      objdir = dynamic_cast<TDirectory*> (f->Get(foldername));

   char namebuf[10000];
   Short_t cyclebuf;
   TDirectory::DecodeNameCycle(objname.Data(), namebuf, cyclebuf, sizeof(namebuf));

   if (objdir) {
      objdir->cd();
      objdir->Delete(objname.Data());
      objdir->WriteTObject(obj, namebuf, "Overwrite");
   }

   f->ReOpen("READ");

   return kTRUE;
}

Bool_t TGo4DirProxy::IsFile() const
{
   return dynamic_cast<TFile*>(fDir) != nullptr;
}

const char* TGo4DirProxy::GetFileName() const
{
   return IsFile() ? fDir->GetName() : nullptr;
}
