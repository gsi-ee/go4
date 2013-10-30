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

#include "TROOT.h"
#include "TGraph.h"
#include "TAxis.h"

#include "dabc/Hierarchy.h"
#include "dabc/Manager.h"
#include "dabc/Publisher.h"
#include "dabc/Configuration.h"


bool IsRateHistory(const dabc::Hierarchy& item)
{
   if ((item.GetField("dabc:kind").AsStr() == "rate") ||
       (item.GetField("dabc:history").AsInt() > 0)) return kTRUE;
   return kFALSE;
}

TString GetRootClassName(const dabc::Hierarchy& item)
{
   std::string kind = item.GetField("dabc:kind").AsStr();
   if (kind.find("ROOT.") == 0) {
      kind.erase(0,5);
      return kind.c_str();
   }
   if (IsRateHistory(item)) return "TGraph";

   return "";
}


class TGo4DabcAccess : public TGo4Access {
   protected:
      dabc::Hierarchy  fItem;
      std::string      fNode;
      static TString   fClNameBuf;

   public:
      TGo4DabcAccess(const dabc::Hierarchy& item, const std::string& node) :
         TGo4Access(),
         fItem(item),
         fNode(node)
      {
         // printf("Create %s\n",fItem.GetName());
      }

      virtual ~TGo4DabcAccess() {}

      virtual Bool_t CanGetObject() const
      {
         // printf("Test 1 %s\n",fItem.GetName());

         if (IsRateHistory(fItem)) return kTRUE;

         return kFALSE;
      }

      virtual Bool_t GetObject(TObject* &obj, Bool_t &owner) const
      {
         if (!IsRateHistory(fItem)) return kFALSE;

         // printf("GO4 WANT ITEM %s\n", fItem.ItemName().c_str());

         dabc::CmdPublisherGet cmd2;
         cmd2.SetStr("Item", fItem.ItemName());
         cmd2.SetUInt("history", fItem.GetField("dabc:history").AsInt(100));
         cmd2.SetTimeout(5.);
         cmd2.SetReceiver(fNode + dabc::Publisher::DfltName());

         if (dabc::mgr.GetCommandChannel().Execute(cmd2)!=dabc::cmd_true) {
            printf("Fail to get item\n");
            return kFALSE;
         }

         dabc::Hierarchy res;
         res.Create("get");
         res.SetVersion(cmd2.GetUInt("version"));
         res.ReadFromBuffer(cmd2.GetRawData());

         //DOUT0("GET:%s RES = \n%s", fItem.ItemName().c_str(), res.SaveToXml(dabc::xmlmask_History).c_str());

         dabc::HistoryIter iter = res.MakeHistoryIter();
         int cnt = 0;
         while (iter.next()) cnt++;

         // DOUT0("ITERATOR cnt %d", cnt);

         if (cnt>0) {
            TGraph* gr = new TGraph(cnt);
            gr->SetName(fItem.GetName());
            gr->SetTitle(Form("%s ratemeter", fItem.ItemName().c_str()));
            int i = 0;
            while (iter.next()) {

               double v = iter.GetField("value").AsDouble();
               uint64_t tm = iter.GetField("time").AsUInt();

               //DOUT0("pnt %d  tm %20u value %5.2f", i, tm / 1000, v);

               gr->SetPoint(cnt - i -1, tm / 1000, v);
               i++;
            }

            gr->GetXaxis()->SetTimeDisplay(1);
            gr->GetXaxis()->SetTimeFormat("%H:%M:%S%F1970-01-01 00:00:00");


            //DOUT0("Graph is created");
            obj = gr;
            owner = kTRUE;
            return kTRUE;
         }


         return kFALSE;
      }

      virtual TClass* GetObjectClass() const
      {
         // printf("Get class\n");
         fClNameBuf = GetRootClassName(fItem);
         if (fClNameBuf.Length()>0) return (TClass*) gROOT->GetListOfClasses()->FindObject(fClNameBuf.Data());
         return 0;
      }

      virtual const char* GetObjectName() const
      {
         return fItem.GetName();
      }

      virtual const char* GetObjectClassName() const
      {
         // printf("Get class name\n");

         fClNameBuf = GetRootClassName(fItem);
         if (fClNameBuf.Length()>0) return fClNameBuf.Data();

         return "dabc::Hierarchy";
      }
};

TString TGo4DabcAccess::fClNameBuf = "";

// ===================================================================================

class TGo4DabcLevelIter : public TGo4LevelIter {
   protected:
      dabc::Hierarchy fParent;
      dabc::Hierarchy fChild;
      TString fClNameBuf; //! buffer used for class name
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

      virtual Int_t GetKind() {
         if (isfolder()) return TGo4Access::kndFolder;

         if (IsRateHistory(fChild)) return TGo4Access::kndObject;

         return -1;
      }

      virtual const char* GetClassName()
      {
         if (IsRateHistory(fChild)) return "TGraph";

         fClNameBuf = GetRootClassName(fChild);

         if (fClNameBuf.Length()>0) return fClNameBuf.Data();

         return "dabc::Hierarchy";
      }
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

   // printf("Connection to %s done\n", nodename);

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

   dabc::Command cmd2("GetGlobalNamesList");
   cmd2.SetReceiver(std::string(fNodeName.Data()) + dabc::Publisher::DfltName());
   cmd2.SetTimeout(5.);

   if (dabc::mgr.GetCommandChannel().Execute(cmd2)!=dabc::cmd_true) {
      printf("Fail to get hierarchy\n");
      return kFALSE;
   }

   dabc::Buffer buf = cmd2.GetRawData();

   if (buf.null()) return kFALSE;

   hierarchy.Release();

      // DOUT0("Get raw data %p %u", buf.SegmentPtr(), buf.GetTotalSize());
   if (!hierarchy.ReadFromBuffer(buf)) return kFALSE;

   //printf("Read from DABC node %s done\n", fNodeName.Data());
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
   //printf("Make PROXY\n");

   if (fxHierarchy == 0) return 0;

   dabc::Hierarchy& hierarchy = *((dabc::Hierarchy*) fxHierarchy);

   if (hierarchy.null()) return 0;

   if ((name==0) || (strlen(name)==0)) {
      //printf("Create access to current item %s\n", hierarchy.GetName());
      return new TGo4DabcAccess(hierarchy, fNodeName.Data());
   }

   dabc::Hierarchy child = hierarchy.FindChild(name);

   //printf("Create access to child %s ptr %p\n", name, child());

   return child.null() ? 0 : new TGo4DabcAccess(child, fNodeName.Data());
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
   return kFALSE;
}

Bool_t TGo4DabcProxy::UpdateHierarchy()
{
   return kFALSE;
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
