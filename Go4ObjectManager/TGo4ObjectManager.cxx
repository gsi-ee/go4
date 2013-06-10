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

#include "TGo4ObjectManager.h"

#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TTimer.h"

#include "TGo4ObjectProxy.h"
#include "TGo4Iter.h"
#include "TGo4DirProxy.h"
#include "TGo4TreeProxy.h"
#include "TGo4FolderProxy.h"
#include "TGo4LinkProxy.h"

class TGo4ObjManLink : public TObject {
   public:
      TGo4ObjManLink(TGo4Slot* source, TGo4Slot* target, Bool_t expandchilds) :
         TObject(),
         fxSource(source),
         fxTarget(target),
         fbExapndChilds(expandchilds)
      {
      }

      Bool_t CheckEventSource(const TGo4Slot* evtsource)
      {
          if (evtsource==fxSource) return kTRUE;
          if (fbExapndChilds && evtsource->IsParent(fxSource)) return kTRUE;
          return kFALSE;
      }

      TGo4Slot* GetSource() const { return fxSource; }
      TGo4Slot* GetTarget() const { return fxTarget; }
      Bool_t DoChildsExpand() const { return fbExapndChilds; }

   protected:
      TGo4Slot*  fxSource;       //!
      TGo4Slot*  fxTarget;       //!
      Bool_t     fbExapndChilds; //!
};

class TGo4ObjManCleanup : public TObject {
   public:
      TGo4ObjManCleanup(TObject* obj, TGo4Slot* slot) :
         TObject(),
         fObject(obj),
         fSlot(slot)
      {
      }
      TObject* GetObject() const { return fObject; }
      TGo4Slot* GetSlot() const { return fSlot; }
   protected:
      TObject*   fObject; //!
      TGo4Slot*  fSlot;   //!
};


// ********************************************************************

TGo4ObjectManager::TGo4ObjectManager() :
   TGo4Slot(),
   fLinks(),
   fCleanups()
{
   gROOT->GetListOfCleanups()->Add(this);
}

TGo4ObjectManager::TGo4ObjectManager(const char* name, const char* title) :
   TGo4Slot(0, name, title),
   fLinks(),
   fCleanups()
{
   gROOT->GetListOfCleanups()->Add(this);
}

TGo4ObjectManager::~TGo4ObjectManager()
{
   DeleteChilds();

   fLinks.Delete();

   fCleanups.Delete();

   gROOT->GetListOfCleanups()->Remove(this);
}

void TGo4ObjectManager::ProduceFullName(TString& name, TGo4Slot* toparent)
{
   name = "";
}

TGo4ObjectManager* TGo4ObjectManager::GetOM() const
{
   return (TGo4ObjectManager*) this;
}

void TGo4ObjectManager::MakeFolder(const char* pathname)
{
   if ((pathname!=0) && (*pathname!=0))
     GetSlot(pathname, kTRUE);
}

TGo4Slot* TGo4ObjectManager::Add(const char* pathname, TObject* obj, Bool_t owner, Bool_t canrename)
{
   if (obj==0) return 0;

   TGo4Slot* slot = MakeObjSlot(pathname, obj->GetName(), obj->ClassName());

   if (slot!=0) {
      if (canrename && (strcmp(obj->GetName(),slot->GetName())!=0)) {
         TNamed* n = dynamic_cast<TNamed*> (obj);
         if (n!=0) n->SetName(slot->GetName());
      }

     slot->SetProxy(new TGo4ObjectProxy(obj, owner));
   }

   return slot;
}

void TGo4ObjectManager::AddFile(const char* pathname, const char* filename)
{
   AddDir(pathname, TFile::Open(filename), kTRUE, kTRUE);
}

void TGo4ObjectManager::CloseFiles(const char* pathname)
{
   TGo4Slot* slot = GetSlot(pathname);
   if (slot==0) return;
   for(int n=slot->NumChilds()-1;n>=0;n--) {
      TGo4Slot* subslot = slot->GetChild(n);
      TGo4DirProxy* dirproxy = dynamic_cast<TGo4DirProxy*> (subslot->GetProxy());
      if (dirproxy!=0)
        if (dirproxy->IsFile())
          delete subslot;
   }
}


void TGo4ObjectManager::AddDir(const char* pathname, TDirectory* dir, Bool_t owner, Bool_t readright)
{
   if (dir==0) return;

   const char* name = (dir->InheritsFrom(TFile::Class())) ?
      gSystem->BaseName(dir->GetName()) : dir->GetName();

   TGo4Slot* slot = MakeObjSlot(pathname, name, dir->ClassName());

   if (slot!=0)
     slot->SetProxy(new TGo4DirProxy(dir, readright, owner));
}


void TGo4ObjectManager::AddTree(const char* pathname, TTree* tree, Bool_t owner)
{
   if (tree==0) return;

   TGo4Slot* slot = MakeObjSlot(pathname, tree->GetName(), tree->ClassName());

   if (slot!=0)
     slot->SetProxy(new TGo4TreeProxy(tree, owner));
}

void TGo4ObjectManager::AddFolder(const char* pathname, TFolder* f, Bool_t owner)
{
   if (f==0) return;

   TGo4Slot* slot = MakeObjSlot(pathname, f->GetName(), f->ClassName());
   if (slot!=0)
     slot->SetProxy(new TGo4FolderProxy(f, owner, ""));
}

void TGo4ObjectManager::AddROOTFolder(const char* pathname, const char* foldername)
{
   TFolder* f = TGo4FolderProxy::LocateROOTFolder(foldername);
   if (f==0) return;

   TGo4Slot* slot = MakeObjSlot(pathname, f->GetName(), f->ClassName());

   if (slot!=0)
     slot->SetProxy(new TGo4FolderProxy(f, kFALSE, foldername));
}

void TGo4ObjectManager::AddROOTFolders(const char* pathname, Bool_t selected)
{
   if (selected) {
      TString name(pathname);
      if (name.Length()>0) name+="/root";
                      else name="root";
      TGo4Slot* slot = GetSlot(name, kTRUE);
      if (slot==0) return;
      slot->SetTitle("ROOT folders");
      AddROOTFolder(name, "//root/Canvases");
      AddROOTFolder(name, "//root/Functions");
      AddROOTFolder(name, "//root/Tasks");
      AddROOTFolder(name, "//root/Specials");
      AddROOTFolder(name, "//root/ROOT Memory");
      AddROOTFolder(name, "//root/ROOT Files");
//      AddDir(name, gROOT, kFALSE);
   } else
      AddROOTFolder(pathname, "//root/");
}

void TGo4ObjectManager::AddProxy(const char* pathname, TGo4Proxy* cont, const char* name, const char* title)
{
   TGo4Slot* slot = MakeObjSlot(pathname, name, title);
   if (slot!=0) slot->SetProxy(cont);
           else delete cont;
}

TGo4Proxy* TGo4ObjectManager::GetProxy(const char* name)
{
   TGo4Slot* slot = GetSlot(name);
   return slot==0 ? 0 : slot->GetProxy();
}


TGo4Slot* TGo4ObjectManager::MakeObjSlot(const char* foldername, const char* name, const char* title)
{
   TGo4Slot* folder = GetSlot(foldername, kTRUE);
   if (folder==0) return 0;
   if (folder->FindChild(name)==0)
     return new TGo4Slot(folder, name, title);

   TString extraname;
   Int_t cycle = 1;

   do {
     extraname.Form("%s_v%d", name, cycle++);
   } while (folder->FindChild(extraname.Data())!=0);

   return new TGo4Slot(folder, extraname.Data(), title);
}


TGo4Slot* TGo4ObjectManager::AddLink(TGo4Slot* source, const char* pathname, const char* linkname, const char* linktitle)
{
   if (source==0) return 0;

   TGo4Slot* slot = MakeObjSlot(pathname, linkname, linktitle);

//   cout << "TGo4ObjectManager::AddLink in " << pathname << " source = " << source->GetFullName() << endl;

   if (slot!=0)
     slot->SetProxy(new TGo4LinkProxy(source));

   for(Int_t indx=fLinks.GetLast(); indx>=0; indx--) {
      TGo4ObjManLink* link = (TGo4ObjManLink*) fLinks.At(indx);
      if (link==0) continue;

      TString namesrc, nametgt;
      link->GetSource()->ProduceFullName(namesrc);
      link->GetTarget()->ProduceFullName(nametgt);

//      cout << "indx = " << indx
//          << "  source = " << namesrc << " target = " << nametgt << endl;
   }

   return slot;
}


TGo4Slot* TGo4ObjectManager::AddLink(TGo4Slot* source, const char* pathname)
{

   if (source==0) return 0;

   TGo4Slot* slot = MakeObjSlot(pathname, source->GetName(), source->GetTitle());

//   cout << "Make link in path " << pathname << " with name " << source->GetName() << endl;

//   if (slot!=0) cout << " slot = " << slot->GetName() <<
//                        "  parent = " << slot->GetParent()->GetName() << endl;

   if (slot!=0)
     slot->SetProxy(new TGo4LinkProxy(source));

   return slot;
}

TGo4Slot* TGo4ObjectManager::AddLink(const char* sourcename, const char* pathname)
{
   return AddLink(GetSlot(sourcename), pathname);
}

TGo4Slot* TGo4ObjectManager::GetLinked(TGo4Slot* link)
{
   TGo4LinkProxy* linkcont = (link==0) ? 0 :
      dynamic_cast<TGo4LinkProxy*> (link->GetProxy());

   return (linkcont!=0) ? linkcont->GetLink() : 0;
}


void TGo4ObjectManager::RegisterLink(TGo4Slot* source, TGo4Slot* target, Bool_t exapndchilds)
{
   fLinks.Add(new TGo4ObjManLink(source, target, exapndchilds));
}

void TGo4ObjectManager::UnregisterLink(TGo4Slot* target)
{
   RemoveFromLinks(target);
}

void TGo4ObjectManager::RemoveFromLinks(const TGo4Slot* slot)
{
   Bool_t docompress = kFALSE;
   for (Int_t n=0;n<=fLinks.GetLast();n++) {
     TGo4ObjManLink* link = (TGo4ObjManLink*) fLinks[n];
     if ((link->GetTarget()==slot) || (link->GetSource()==slot)) {
         fLinks.Remove(link);
         delete link;
         docompress = kTRUE;
     }
   }
   if (docompress)
     fLinks.Compress();
}

void TGo4ObjectManager::RetranslateEvent(TGo4Slot* source, Int_t id, void* param)
{
   if (source==0) return;

   for(Int_t indx=fLinks.GetLast(); indx>=0; indx--) {
      TGo4ObjManLink* link = (TGo4ObjManLink*) fLinks.At(indx);
      if (link==0) continue;

      if (link->CheckEventSource(source)) {
         TGo4Slot* target = link->GetTarget();

         if (gDebug>2)
            Info("RetranslateEvent","src = %p %s tgt = %p %s id = %d", source, source->GetFullName().Data(), target, target->GetFullName().Data(), id);

         target->Event(source, id, param);
      }
   }
}

void TGo4ObjectManager::Event(TGo4Slot* source, Int_t id, void* param)
{
   if (gDebug>2)
      Info("Event","src %s id %d", source->GetFullName().Data(), id);

   RetranslateEvent(source, id, param);

   if (id==evDelete) {
      RemoveFromLinks(source);
      UnregisterObject(0, (TGo4Slot*) source);
   }

   TGo4Slot::Event(source, id, param);
}

void TGo4ObjectManager::SaveDataToFile(TFile* f, Bool_t onlyobjs, TGo4Slot* startslot)
{
   Bool_t usefile = (f!=0);

   TDirectory* olddir = gDirectory;

   TDirectory* curdir = f;

   if (startslot==0) startslot = this;

   TGo4Iter iter(startslot, kTRUE);

   bool isxml = (f!=0) && f->InheritsFrom("TXMLFile");

   while (iter.next()) {

      if (usefile && !isxml) {
         Int_t levelchange = iter.levelchange();

         while ((levelchange++<0) && (curdir!=0)) {
             curdir = dynamic_cast<TDirectory*> (curdir->GetMother());
         }
         if (curdir==0) break;

         if (iter.isfolder()) {
            curdir = curdir->mkdir(iter.getname(),"subdirectory");
         }
         if (curdir==0) break;
      }

      TGo4Slot* slot = iter.getslot();
      if (slot!=0)
         slot->SaveData(curdir, onlyobjs);
   }

   if (olddir!=0) olddir->cd();
}

void TGo4ObjectManager::ReadDataFromFile(TFile* f)
{
   Bool_t usefile = (f!=0);

   TDirectory* olddir = gDirectory;

   TDirectory* curdir = f;

   TGo4Iter iter(this, kTRUE);

   while (iter.next()) {
      if (usefile) {
         Int_t levelchange = iter.levelchange();
         while ((levelchange++<0) && (curdir!=0))
             curdir = dynamic_cast<TDirectory*> (curdir->GetMother());
         if (curdir==0) break;
         if (iter.isfolder())
             curdir->GetObject(iter.getname(), curdir);
         if (curdir==0) break;
      }

      TGo4Slot* slot = iter.getslot();
      if (slot!=0)
         slot->ReadData(curdir);
   }

   if (olddir!=0) olddir->cd();
}

void TGo4ObjectManager::RegisterObjectWith(TObject* obj, TGo4Slot* slot)
{
   if (obj==0) return;
   fCleanups.Add(new TGo4ObjManCleanup(obj, slot));
   obj->SetBit(kMustCleanup);
}

void TGo4ObjectManager::UnregisterObject(TObject* obj, TGo4Slot* slot)
{
   Bool_t compress = kFALSE;
   for(int indx=fCleanups.GetLast();indx>=0;indx--) {
      TGo4ObjManCleanup* entry = (TGo4ObjManCleanup*) fCleanups.At(indx);
      if (entry->GetSlot()!=slot) continue;
      if ((obj==0) || (entry->GetObject()==obj)) {
         fCleanups.Remove(entry);
         delete entry;
         compress = kTRUE;
      }
   }
   if (compress)
      fCleanups.Compress();
}

void TGo4ObjectManager::RecursiveRemove(TObject* obj)
{
   if ((obj==0) || (obj==this)) return;

   Bool_t compress = kFALSE;
   for(int indx=fCleanups.GetLast();indx>=0;indx--) {
      TGo4ObjManCleanup* entry = (TGo4ObjManCleanup*) fCleanups.At(indx);
      if (entry==0) continue;
      if (entry->GetObject()==obj) {
         // first
         fCleanups.Remove(entry);

         entry->GetSlot()->RecursiveRemove(obj);
//         cout << "TGo4ObjectManager::RecursiveRemove " << entry->GetSlot()->GetFullName() << "  " << obj->GetName() << endl;
         entry->GetSlot()->ForwardEvent(entry->GetSlot(), evObjDeleted, obj);
         delete entry;
         compress = kTRUE;
      }
   }
   if (compress)
      fCleanups.Compress();
}

void TGo4ObjectManager::PrintSlots()
{
   TGo4Iter iter(this, kTRUE);
   while (iter.next()) {

      printf("%*c%s\n", (iter.level()+1)*2, ' ', iter.getname());

//      if (iter.getslot()!=0)
//          iter.getslot()->PrintPars((iter.level()+1)*2 + 3);
   }
}

Int_t TGo4ObjectManager::IterateSlots()
{
   TGo4Iter iter(this);
   Int_t cnt = 0;
   while (iter.next()) cnt++;
   return cnt;
}

void TGo4ObjectManager::DeleteSlot(const char* pathname)
{
   TGo4Slot* slot = (TGo4Slot*) GetSlot(pathname);
   if (slot!=0) delete slot;
}

Int_t TGo4ObjectManager::RequestObject(const char* source, const char* targetslot, Int_t waittime_millisec)
// returns 0 when error
//         1 when object assigned immediately
//         2 when object will be obtained later
{
   TGo4Slot* tgtslot = GetSlot(targetslot);
   if (tgtslot==0) return 0;

   TGo4Access* proxy = ProvideSlotAccess(source);
   if (proxy==0) return 0;

   TClass* cl = proxy->GetObjectClass();
   if (cl==0) return 0;

   tgtslot->ResetAssignFlag();

   Int_t res = proxy->AssignObjectTo(this, targetslot);

   if (res<2) delete proxy;

   if ((res==2) && (waittime_millisec>0)) {

      gSystem->ProcessEvents();

      while ((tgtslot->GetAssignFlag()<0) && (waittime_millisec>0)) {
         gSystem->Sleep(10);
         waittime_millisec-=10;
         gSystem->ProcessEvents();
      }

      res = (tgtslot->GetAssignFlag() == (Int_t) kTRUE) ? 1 : 0;
   }

   return res;
}

Bool_t TGo4ObjectManager::AssignObject(const char* path, TObject* obj, Bool_t ownership)
{
   Bool_t res = kFALSE;
   TGo4Slot* tgtslot = GetSlot(path);
   if (tgtslot!=0) res = tgtslot->AssignObject(obj, ownership);
              else if (ownership) delete obj;
   return res;
}
