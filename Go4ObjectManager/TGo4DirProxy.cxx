#include "TGo4DirProxy.h"

#include "TSystem.h"
#include "TDirectory.h"
#include "TKey.h"
#include "TROOT.h"
#include "TH1.h"
#include "TTree.h"
#include "TFile.h"

#include "TGo4ObjectProxy.h"
#include "TGo4TreeProxy.h"
#include "TGo4CanvasProxy.h"
#include "TGo4Slot.h"

class TGo4KeyAccess : public TGo4Access {
   public:
      TGo4KeyAccess(TDirectory* dir, TKey* key) : TGo4Access(), fDir(dir), fKey(key) {}

      virtual Bool_t CanGetObject() const
        { return kTRUE; }

      virtual Bool_t GetObject(TObject* &obj, Bool_t &owner) const
      {
         TClass* cl = gROOT->GetClass(fKey->GetClassName());
         if ((cl!=0) && !cl->IsLoaded()) {
            obj = 0;
            owner = kFALSE;
            return kFALSE;
         }

         owner = kTRUE;
         fDir->cd();
         obj = fKey->ReadObj();
         if (obj!=0)
           if (obj->InheritsFrom(TH1::Class()) || obj->InheritsFrom(TTree::Class()))
             owner = kFALSE;
         return obj!=0;
      }

      virtual const char* GetObjectName() const
        { return fKey->GetName(); }

      virtual const char* GetObjectClassName() const
        { return fKey->GetClassName(); }

   private:
      TDirectory*  fDir;     //!
      TKey*        fKey;     //!
};

// ************************************************************************

class TGo4DirLevelIter : public TGo4LevelIter {
   protected:
      TDirectory*    fDir;           //!
      Bool_t         fReadRight;     //!
      TIterator*     fIter;          //!
      Bool_t         fIsKeyIter;     //!
      TObject*       fCurrent;       //!
      TString        fNameBuf;       //!

   public:
      TGo4DirLevelIter(TDirectory* dir, Bool_t readright) :
         TGo4LevelIter(),
         fDir(dir),
         fReadRight(readright),
         fIter(0),
         fIsKeyIter(kFALSE),
         fCurrent(0),
         fNameBuf()
      {
         fIter = fDir->GetListOfKeys()->MakeIterator();
         fIsKeyIter = kTRUE;
      }

      virtual ~TGo4DirLevelIter()
      {
         if (fIter!=0) delete fIter;
      }

      virtual Bool_t next()
      {
         Bool_t donext = kTRUE;

         while (donext) {

            fCurrent = fIter->Next();

            if (fCurrent==0){
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
               if (key!=0)
                 if (strcmp(fCurrent->ClassName(), key->GetClassName())==0) donext = kTRUE;
            }
         }
         return fCurrent!=0;
      }

      virtual Bool_t isfolder()
      {
         TClass* cl = 0;
         if (fIsKeyIter) {
            TKey* key = (TKey*) fCurrent;
            if (fReadRight)
              cl = (TClass*) gROOT->GetListOfClasses()->FindObject(key->GetClassName());
         } else
            cl = fCurrent->IsA();

         return (cl!=0) &&
                (cl->InheritsFrom(TDirectory::Class()) ||
                 cl->InheritsFrom(TTree::Class()) ||
                 cl->InheritsFrom(TCanvas::Class()));
      }

      virtual TGo4LevelIter* subiterator()
      {
         TObject* obj = fIsKeyIter ? fDir->Get(fCurrent->GetName()) : fCurrent;
         if (obj==0) return 0;

         if (obj->InheritsFrom(TTree::Class()))
           return TGo4TreeProxy::ProduceIter((TTree*)obj);

         if (obj->InheritsFrom(TCanvas::Class()))
           return TGo4CanvasProxy::ProduceIter((TCanvas*)obj);

         TDirectory* subdir = dynamic_cast<TDirectory*> (obj);
         return subdir==0 ? 0 : new TGo4DirLevelIter(subdir, fReadRight);
      }

      virtual const char* name()
      {
         if (!fIsKeyIter) return fCurrent->GetName();
         TKey* key = (TKey*) fCurrent;
         if (isfolder() || (key->GetCycle()==1)) return key->GetName();
         fNameBuf.Form("%s;%d",key->GetName(),key->GetCycle());
         return fNameBuf.Data();
      }

      virtual const char* info()
      {
         return fCurrent->GetTitle();
      }

      virtual Int_t sizeinfo()
      {
         TClass* cl = 0;
         Int_t sz = 0;
         if (fIsKeyIter) {
            TKey* key = (TKey*) fCurrent;
            cl = (TClass*) gROOT->GetListOfClasses()->FindObject(key->GetClassName());
            sz = key->GetNbytes();
         } else {
            cl = fCurrent->IsA();
            sz = cl->Size();
         }
         bool isdir = (cl!=0) && cl->InheritsFrom(TDirectory::Class());
         return isdir ? 0 : sz;
      }

      virtual Int_t GetKind()
      {
         return isfolder() ? TGo4Access::kndFolder : TGo4Access::kndObject;
      }

      virtual const char* GetClassName()
      {
          return fIsKeyIter ? ((TKey*) fCurrent)->GetClassName() : fCurrent->ClassName();
      }
};

//*************************************************************************

TGo4DirProxy::TGo4DirProxy() :
   TGo4Proxy(),
   fDir(0),
   fOwner(kFALSE),
   fReadRight(kFALSE)
{
}

TGo4DirProxy::TGo4DirProxy(TDirectory* dir, Bool_t readright, Bool_t owner) :
   TGo4Proxy(),
   fDir(0),
   fOwner(kFALSE),
   fReadRight(kFALSE)
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
   if (fOwner && (fDir!=0)) delete fDir;
   fDir = 0;
   fOwner = kFALSE;
}

Int_t TGo4DirProxy::GetObjectKind()
{
   return (fDir!=0) ? TGo4Access::kndFolder : TGo4Access::kndNone;
}

const char* TGo4DirProxy::GetContainedClassName()
{
   return fDir!=0 ? fDir->ClassName() : 0;
}

const char* TGo4DirProxy::GetContainedObjectInfo()
{
   return fDir!=0 ? fDir->GetTitle() : 0;
}

Int_t TGo4DirProxy::GetObjectSizeInfo()
{
    TFile* f = dynamic_cast<TFile*> (fDir);
    return f==0 ? TGo4Proxy::GetObjectSizeInfo() : f->GetSize();
}


TGo4Access* TGo4DirProxy::ProduceProxy(TDirectory* dir, Bool_t readright, const char* name)
{
   if (dir==0) return 0;

   if ((name==0) || (*name==0)) return new TGo4ObjectAccess(dir);

   TDirectory* curdir = dir;
   const char* curname = name;

   while (curdir!=0) {
      const char* slash = strchr(curname,'/');
      TObject* obj = 0;
      if (slash==0) {
         char namebuf[10000];
         Short_t cyclebuf;
         TDirectory::DecodeNameCycle(curname, namebuf, cyclebuf);
         obj = curdir->GetList()->FindObject(curname);
         if (obj!=0) return new TGo4ObjectAccess(obj);
         if (cyclebuf==9999) cyclebuf=1;
         TKey* key = curdir->GetKey(namebuf, cyclebuf);
         if (key!=0) return new TGo4KeyAccess(curdir, key);
         return 0;
      }

      UInt_t len = slash - curname;
      char* partname = new char[len + 1];
      strncpy(partname, curname, len);
      partname[len] = 0;
      if (readright) {
         curdir->cd();
         obj = curdir->Get(partname);
      } else
         obj = curdir->GetList()->FindObject(partname);
      delete[] partname;

      if (obj==0) return 0;

      curname = slash+1;

      TTree* tr = dynamic_cast<TTree*> (obj);
      if (tr!=0)
         return TGo4TreeProxy::ProduceProxy(tr, curname);

      TCanvas* canv = dynamic_cast<TCanvas*> (obj);
      if (canv!=0)
         return TGo4CanvasProxy::ProduceProxy(canv, curname);

      curdir = dynamic_cast<TDirectory*>(obj);
   }
   return 0;
}

TGo4LevelIter* TGo4DirProxy::ProduceIter(TDirectory* dir, Bool_t readright)
{
   return new TGo4DirLevelIter(dir, readright);
}


void TGo4DirProxy::WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs)
{
   if (!onlyobjs) {
      const char* filename = 0;
      if ((fDir!=0) && fDir->InheritsFrom(TFile::Class()))
         filename = fDir->GetName();

     slot->SetPar("DirProxy::FileName", filename);
     slot->SetPar("DirProxy::gROOT", (fDir==gROOT) ? "true" : 0);
   }
}

void TGo4DirProxy::ReadData(TGo4Slot* slot, TDirectory* dir)
{
   ClearDir();

   const char* filename = slot->GetPar("DirProxy::FileName");
   const char* groot = slot->GetPar("DirProxy::gROOT");

   if (filename!=0) {
     TFile* f = TFile::Open(filename);
     if (f!=0) SetDir(f, kTRUE, kTRUE);
   } else
   if (groot!=0)
     SetDir(gROOT, kFALSE, kFALSE);
}

Bool_t TGo4DirProxy::UpdateObjectInFile(const char* filepath, TObject* obj)
{
   if ((filepath==0) || (fDir==0)) return kFALSE;

   TFile* f = dynamic_cast<TFile*> (fDir);
   if (f==0) return kFALSE;

   if (f->ReOpen("UPDATE")<0) return kFALSE;

   TString foldername, objname;
   TGo4Slot::ProduceFolderAndName(filepath, foldername, objname);

   TDirectory* objdir = f;

   if (foldername.Length()>0) {
      objdir = dynamic_cast<TDirectory*> (f->Get(foldername));
   }

   char namebuf[10000];
   Short_t cyclebuf;
   TDirectory::DecodeNameCycle(objname.Data(), namebuf, cyclebuf);


   if (objdir!=0) {
     objdir->cd();
     objdir->Delete(objname.Data());
     objdir->WriteTObject(obj, namebuf, "Overwrite");
   }

   f->ReOpen("READ");

   return kTRUE;
}

Bool_t TGo4DirProxy::IsFile() const
{
   return dynamic_cast<TFile*>(fDir)!=0;
}

const char* TGo4DirProxy::GetFileName() const
{
   return IsFile() ? fDir->GetName() : 0;
}
