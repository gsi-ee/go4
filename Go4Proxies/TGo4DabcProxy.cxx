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

#include "TGo4DabcProxy.h"

#ifndef WITHOUT_DABC

#include "dabc/Hierarchy.h"
#include "dabc/Manager.h"
#include "dabc/Configuration.h"

class TGo4DabcAccess : public TGo4Access {
   protected:
      dabc::Hierarchy  fItem;

   public:
      TGo4DabcAccess(const dabc::Hierarchy& item) :
         TGo4Access(),
         fItem(item)
      {
         fItem.SetTransient(false);
      }

      virtual ~TGo4DabcAccess() {}

      virtual Bool_t CanGetObject() const { return kFALSE; }
      virtual Bool_t GetObject(TObject* &obj, Bool_t &owner) const { return kFALSE; }
      virtual TClass* GetObjectClass() const { return 0; }
      virtual const char* GetObjectName() const { return fItem.GetName(); }
      virtual const char* GetObjectClassName() const { return "dabc::Hierarchy"; }
};

// ===================================================================================

class TGo4DabcLevelIter : public TGo4LevelIter {
   protected:
      dabc::Hierarchy fParent;
      dabc::Hierarchy fChild;
      unsigned fCnt;

   public:
      TGo4DabcLevelIter(const dabc::Hierarchy& item) :
         TGo4LevelIter(),
         fParent(item),
         fChild(),
         fCnt(0)
         {
         }

      virtual ~TGo4DabcLevelIter() {}

      virtual Bool_t next()
      {
         if (fParent.NumChilds()==0) return kFALSE;

         if (fChild.null()) {
            fCnt = 0;
            fChild = fParent.GetChild(fCnt);
//            printf("Extract first child %s from iter\n", fChild.GetName());
         } else {
            fCnt++;
            fChild.Release();
            if (fCnt>=fParent.NumChilds()) return kFALSE;
            fChild = fParent.GetChild(fCnt);
//            printf("Extract %u child %s from iter\n", fCnt, fChild.GetName());
         }

         fChild.SetTransient(false);

         return !fChild.null();
      }

      virtual Bool_t isfolder() { return fChild.NumChilds()>0; }

      virtual Int_t getflag(const char*) { return -1; }

      virtual TGo4LevelIter* subiterator() {
         return fChild.NumChilds() > 0 ? new TGo4DabcLevelIter(fChild) : 0;
      }

      virtual TGo4Slot* getslot() { return 0; }

      virtual const char* name() { return fChild.GetName(); }
      virtual const char* info() { return "item from dabc"; }
      virtual Int_t sizeinfo() { return 0; }

      virtual Int_t GetKind() { return isfolder() ? TGo4Access::kndFolder : -1; }

      virtual const char* GetClassName() { return "dabc::Hierarchy"; }
};


// =====================================================================================


TGo4DabcProxy::TGo4DabcProxy() :
   TGo4Proxy(),
   fNodeName(),
   fxHierarchy(0)
{
}

TGo4DabcProxy::~TGo4DabcProxy()
{
   if (fxHierarchy!=0) {
      delete ((dabc::Hierarchy*) fxHierarchy);
      fxHierarchy = 0;
   }
}

Bool_t TGo4DabcProxy::Connect(const char* nodename)
{
   if (dabc::mgr.null()) {

      static dabc::Configuration cfg;

      new dabc::Manager("cmd", &cfg);

      // ensure that all submitted events are processed
      dabc::mgr.SyncWorker();

      dabc::mgr()->CreateControl(false);
   }

   dabc::Command cmd("Ping");
   cmd.SetReceiver(nodename);
   cmd.SetTimeout(5.);

   int res = dabc::mgr.GetCommandChannel().Execute(cmd);

   if (res != dabc::cmd_true) {
      printf("FAIL connection to %s\n", nodename);
      fNodeName = nodename;
      return kFALSE;
   }

   printf("Connection to %s done\n", nodename);

   fNodeName = nodename;

   return UpdateHierarchy();
}

Bool_t TGo4DabcProxy::UpdateHierarchy()
{
   if (fNodeName.Length() == 0) return kFALSE;

   if (fxHierarchy==0) {
      fxHierarchy = new dabc::Hierarchy();
   }

   dabc::Hierarchy& hierarchy = *((dabc::Hierarchy*) fxHierarchy);

   dabc::Command cmd("GetHierarchy");
   cmd.SetInt("version", hierarchy.GetVersion());
   cmd.SetReceiver(fNodeName.Data());
   cmd.SetTimeout(5.);

   if (dabc::mgr.GetCommandChannel().Execute(cmd)!=dabc::cmd_true) {
      printf("Fail to get hierarchy\n");
      return kFALSE;
   }

   if (cmd.GetRawDataSize() > 0) {
      // DOUT0("Get raw data %p %u", cmd2.GetRawData(), cmd2.GetRawDataSize());

      std::string diff;
      diff.append((const char*)cmd.GetRawData(), cmd.GetRawDataSize());
      // DOUT0("diff = %s", diff.c_str());
      if (hierarchy.UpdateFromXml(diff)) {
         DOUT0("Update of hierarchy to version %u done", hierarchy.GetVersion());
      }
   }

   return kTRUE;
}

void TGo4DabcProxy::Initialize(TGo4Slot* slot)
{
}

void TGo4DabcProxy::Finalize(TGo4Slot* slot)
{
}


Bool_t TGo4DabcProxy::HasSublevels() const
{
   if (fxHierarchy == 0) return false;

   dabc::Hierarchy& hierarchy = *((dabc::Hierarchy*) fxHierarchy);

   return hierarchy.NumChilds() > 0;
}

TGo4Access* TGo4DabcProxy::MakeProxy(const char* name)
{
   if (fxHierarchy == 0) return 0;

   dabc::Hierarchy& hierarchy = *((dabc::Hierarchy*) fxHierarchy);

   if (hierarchy.null()) return 0;

   if ((name==0) || (strlen(name)==0)) {
//      printf("Create access to current item %s", hierarchy.GetName());
      return new TGo4DabcAccess(hierarchy);
   }

   dabc::Hierarchy child = hierarchy.FindChild(name);

//   printf("Create access to child %s", name);

   return child.null() ? 0 : new TGo4DabcAccess(child);
}

TGo4LevelIter* TGo4DabcProxy::MakeIter()
{
   if (fxHierarchy == 0) return 0;

   dabc::Hierarchy& hierarchy = *((dabc::Hierarchy*) fxHierarchy);

//   printf("TGo4DabcProxy::MakeIter()\n");

   return hierarchy.null() ? 0 : new TGo4DabcLevelIter(hierarchy);
}

void TGo4DabcProxy::WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs)
{

}

void TGo4DabcProxy::ReadData(TGo4Slot* slot, TDirectory* dir)
{

}

void TGo4DabcProxy::Update(TGo4Slot* slot, Bool_t strong)
{
   if (strong) {
      printf("GO4 WANTS update DABC hierarchy - do it");
      UpdateHierarchy();
   }
}

#else

TGo4DabcProxy::TGo4DabcProxy() :
   TGo4Proxy(),
   fxHierarchy(0)
{
}

TGo4DabcProxy::~TGo4DabcProxy()
{
}

Bool_t TGo4DabcProxy::Connect(const char* nodename)
{
   return false;
}

void TGo4DabcProxy::Initialize(TGo4Slot* slot)
{
}

void TGo4DabcProxy::Finalize(TGo4Slot* slot)
{
}

Bool_t TGo4DabcProxy::HasSublevels() const
{
   return kFALSE;
}

TGo4Access* TGo4DabcProxy::MakeProxy(const char* name)
{
   return 0;
}

TGo4LevelIter* TGo4DabcProxy::MakeIter()
{
   return 0;
}

void TGo4DabcProxy::WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs)
{

}

void TGo4DabcProxy::ReadData(TGo4Slot* slot, TDirectory* dir)
{

}

void TGo4DabcProxy::Update(TGo4Slot* slot, Bool_t strong)
{
}

#endif
