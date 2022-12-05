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

#include "TGo4DabcProxy.h"

#ifndef WITHOUT_DABC

#include "TROOT.h"
#include "TGraph.h"
#include "TClass.h"
#include "TAxis.h"
#include "TTimer.h"
#include "TBufferFile.h"
#include "TMemFile.h"
#include "RZip.h"

#include "TGo4Log.h"
#include "TGo4Slot.h"
#include "TGo4ObjectManager.h"

#include "dabc/api.h"
#include "dabc/version.h"
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

// we use fake file only to correctly reconstruct streamer infos
class TFakeFile : public TMemFile {
   protected:
      TList*  mylist;

      InfoListRet GetStreamerInfoListImpl(bool) override
      {
         ROOT::Internal::RConcurrentHashColl::HashValue hash;
         return { (TList*) mylist->Clone(), 0, hash};
      }

   public:

      TFakeFile(TList *sinfo) : TMemFile("dummy.file", "RECREATE"), mylist(sinfo)
      {
         gROOT->GetListOfFiles()->Remove(this);
      }

      virtual ~TFakeFile() {}

};


class TGo4DabcAccess : public TGo4Access {
   protected:
      std::string      fNodeName;
      std::string      fObjName;
      std::string      fItemName;
      std::string      fRootClassName;
      std::string      fMasterName;
      std::string      fMasterItemName;
      bool             fIsRate;
      int              fHistoryLength;
      TGo4ObjectManager* fxReceiver;
      TString             fxRecvPath;
      dabc::Buffer     fRawData;     //! raw data, get from command
      Bool_t           fCompression;   //! request compression from server

   public:

      TGo4DabcAccess(const std::string& node, const std::string& sinfoname) :
         TGo4Access(),
         fNodeName(node),
         fObjName(),
         fItemName(),
         fRootClassName(),
         fMasterName(),
         fMasterItemName(),
         fIsRate(false),
         fHistoryLength(0),
         fxReceiver(nullptr),
         fxRecvPath(),
         fRawData(),
         fCompression(kTRUE)
      {
         fObjName = "StreamerInfo";
         fItemName = sinfoname;
         fRootClassName = "TList";

         //printf("Create MASTER ACCESS %p\n", this);
      }


      TGo4DabcAccess(const std::string& node, const dabc::Hierarchy& item) :
         TGo4Access(),
         fNodeName(node),
         fObjName(),
         fItemName(),
         fRootClassName(),
         fMasterName(),
         fMasterItemName(),
         fIsRate(false),
         fHistoryLength(0),
         fxReceiver(nullptr),
         fxRecvPath(),
         fRawData(),
         fCompression(kTRUE)
      {
         fObjName = item.GetName();
         fItemName = item.ItemName();
         std::string kind = item.GetField(dabc::prop_kind).AsStr();
         fHistoryLength = item.GetField(dabc::prop_history).AsInt();

         fMasterName = item.GetField(dabc::prop_masteritem).AsStr();
         fMasterItemName = item.FindMaster().ItemName();

         if (kind.find("ROOT.") == 0) {
            kind.erase(0,5);
            fRootClassName = kind;
         } else
         if (kind=="rate") {
            fIsRate = true;
            if (fHistoryLength > 0) fRootClassName = "TGraph";
         }

         //printf("Create ACCESS %p\n", this);
      }

      virtual ~TGo4DabcAccess()
      {
         //printf("Destroy ACCESS %p\n", this);
      }

      Bool_t IsRemote() const override { return kTRUE; }

      Bool_t CanGetObject() const override { return kFALSE; }

      Bool_t GetObject(TObject* &obj, Bool_t &owner) const override { return kFALSE; }

      TClass *GetObjectClass() const override
      {
         if (fRootClassName.length() > 0)
            return TGo4Proxy::GetClass(fRootClassName.c_str());
         return nullptr;
      }

      const char *GetObjectName() const override
      {
         return fObjName.c_str();
      }

      const char *GetObjectClassName() const override
      {
         if (fRootClassName.length()>0) return fRootClassName.c_str();

         return "dabc::Hierarchy";
      }


      Int_t AssignObjectTo(TGo4ObjectManager* rcv, const char *path) override
      {
         if (!rcv) return 0;

         dabc::WorkerRef wrk = dabc::mgr.FindItem("/Go4ReplWrk");
         if (wrk.null()) return 0;

         if (fIsRate && (fHistoryLength>0)) {
            dabc::CmdGetBinary cmd2;
            cmd2.SetStr("Item", fItemName);
            cmd2.SetStr("Kind","hierarchy");
            cmd2.SetStr("Query", TString::Format("history=%d",fHistoryLength).Data());
            cmd2.SetTimeout(10.);
            cmd2.SetReceiver(fNodeName + dabc::Publisher::DfltName());

            cmd2.SetPtr("#DabcAccess", this);
            fxReceiver = rcv;
            fxRecvPath = path;
            wrk()->Assign(cmd2);

            if (dabc::mgr.GetCommandChannel().Submit(cmd2)) return 2;
         } else

         if (!fRootClassName.empty()) {

            TClass *cl = TClass::GetClass(fRootClassName.c_str());
            if (!cl) {
               TGo4Log::Error("GetObject Unknown class %s", fRootClassName.c_str());
               return 0;
            }

            if (!cl->InheritsFrom(TObject::Class())) {
              // in principle user should call TKey::ReadObjectAny!
               TGo4Log::Error("GetObject Class %s not derived from TObject", fRootClassName.c_str());
               return 0;
            }

            dabc::CmdGetBinary cmd(fItemName, "root.bin", fCompression ? "zipped" : "");
            // cmd.SetStr("Item", fItemName);
            // cmd.SetUInt("version", version);
            cmd.SetTimeout(10.);
            cmd.SetReceiver(fNodeName + dabc::Publisher::DfltName());

            cmd.SetPtr("#DabcAccess", this);
            fxReceiver = rcv;
            fxRecvPath = path;

            //printf("Request for recv %p path %s master %s\n", rcv, path, fMasterName.c_str());
            wrk()->Assign(cmd);

            if (dabc::mgr.GetCommandChannel().Submit(cmd)) return 2;
         }

         return 0;
      }

      /** Returns repetition timeout, negative - delete access object */
      double ProcessCommandReply(dabc::Command cmd)
      {
         // DOUT0("ProcessCommandReply");

         if (cmd.GetResult() != dabc::cmd_true) {
            TGo4Log::Error("dabc::Command %s execution failed", cmd.GetName());
            return -1;
         }

         if (fRawData.null())
            fRawData = cmd.GetRawData();

         if (fRawData.null() || (fRawData.NumSegments() == 0)) {
           TGo4Log::Error("Did not get raw data from dabc::Command %s", cmd.GetName());
           return -1;
         }

         if (fIsRate && (fHistoryLength>0)) {
            dabc::Hierarchy res;
            res.Create("get");
            res.SetVersion(cmd.GetUInt("version"));
            res.ReadFromBuffer(fRawData);

            //DOUT0("GET:%s RES = \n%s", item.ItemName().c_str(), res.SaveToXml(dabc::xmlmask_History).c_str());

            dabc::HistoryIter iter = res.MakeHistoryIter();
            int cnt = 0;
            while (iter.next()) cnt++;

            // DOUT0("ITERATOR cnt %d", cnt);

            if (cnt == 0) return kFALSE;

            TGraph* gr = new TGraph(cnt);
            gr->SetName(fObjName.c_str());
            gr->SetTitle(TString::Format("%s ratemeter", fItemName.c_str()).Data());
            int i = 0;
            while (iter.next()) {

               double v = iter.GetField("value").AsDouble();
               uint64_t tm = iter.GetField("time").AsUInt();

               // DOUT0("pnt %d  tm %20u value %5.2f", i, tm / 1000, v);

               i++;
               gr->SetPoint(cnt-i, tm / 1000, v);
            }

            gr->GetXaxis()->SetTimeDisplay(1);
            gr->GetXaxis()->SetTimeFormat("%H:%M:%S%F1970-01-01 00:00:00");

            DoObjectAssignement(fxReceiver, fxRecvPath.Data(), gr, kTRUE);
            return -1.;
         }

         if (!fRootClassName.empty()) {
            TClass *cl = TClass::GetClass(fRootClassName.c_str());

            // dabc::BinDataHeader* hdr = (dabc::BinDataHeader*) fRawData.SegmentPtr();

            TGo4Slot *tgtslot = fxReceiver->GetSlot(fxRecvPath.Data());

            TGo4Slot *masterslot = nullptr;

            if (!fMasterName.empty() && tgtslot && tgtslot->GetParent())
               masterslot = tgtslot->GetParent()->FindSlot(fMasterName.c_str());

            if (masterslot) {
               // always request master

               Int_t local_master_version = 0;

               if (!masterslot->GetIntPar("dabc_version", local_master_version))
                  local_master_version = 0;

               if (local_master_version >= cmd.GetInt("MVersion")) {
                  //printf("MASTER version %d required %d EXISTS!!!\n", local_master_version, cmd.GetInt("MVersion"));
               } else
               if (masterslot->GetPar("dabc_loading")) {
                  // if master slot loading, we will try to check again 1 sec later
                  return 1.;
               } else {

                  TGo4DabcAccess* maccess = new TGo4DabcAccess(fNodeName, fMasterItemName);

                  if (maccess->AssignObjectToSlot(fxReceiver) != 2) {
                     delete maccess;
                     TGo4Log::Error("Fail to request MASTER item %s from DABC node %s", fMasterItemName.c_str(), fNodeName.c_str());
                     return -1;
                  }

                  masterslot->SetPar("dabc_loading", "true");
                  return 1.; // try after 1 seconds
               }
            }

            TGo4Log::Debug("Item %s raw_data size %u master_version %u", fItemName.c_str(), fRawData.GetTotalSize(), cmd.GetInt("MVersion"));

            char *pobj = (char*) cl->New();

            if (!pobj) {
               TGo4Log::Error("ReadObj - Cannot create new object of class %s", cl->GetName());
               return true;
            }

            Int_t baseOffset = cl->GetBaseClassOffset(TObject::Class());
            if (baseOffset==-1) {
               // cl does not inherit from TObject.
               // Since this is not possible yet, the only reason we could reach this code
               // is because something is screw up in the ROOT code.
               TGo4Log::Error("ReadObj Incorrect detection of the inheritance from TObject for class %s.",
                     cl->GetName());
               return -1;
            }

            TObject *tobj = (TObject*)(pobj+baseOffset);
              // Create an instance of this class

            char* rawbuf = nullptr;
            Int_t rawbuflen = 0;

            if (fCompression) {

               int sizeinp(fRawData.GetTotalSize()), sizeout(0), irep(0);

               if (R__unzip_header(&sizeinp, (unsigned char*) fRawData.SegmentPtr(), &sizeout)) {
                  printf("Fail to decode zip header\n");
               } else {
                  rawbuf = (char*) malloc(sizeout);

                  R__unzip(&sizeinp, (unsigned char*) fRawData.SegmentPtr(), &sizeout, (unsigned char*) rawbuf, &irep);

                  rawbuflen = irep;
               }
            } else {
               rawbuf = (char*) fRawData.SegmentPtr();
               rawbuflen = fRawData.GetTotalSize();
            }

            if (rawbuflen>0) {
               TBufferFile buf(TBuffer::kRead, rawbuflen, rawbuf, kFALSE);
               buf.MapObject(pobj,cl);
               tobj->Streamer(buf);
            } else {
               cl->Destructor(pobj);
               pobj = nullptr;
               tobj = nullptr;
            }

            if (fCompression) free(rawbuf);

            if ((fObjName == "StreamerInfo") &&
                (fRootClassName == "TList") &&
                tobj && tobj->InheritsFrom(TList::Class()) &&
                fMasterName.empty()) {

               TFakeFile fff((TList*) tobj);
               fff.ReadStreamerInfo();

               TGo4Log::Debug("Get streamer infos from remote");
            }

            if (tobj) {
               if (tgtslot) {
                  tgtslot->RemovePar("dabc_loading");  // indicate that loading is finished
                  tgtslot->SetIntPar("dabc_version", cmd.GetInt("BVersion"));
               }
               DoObjectAssignement(fxReceiver, fxRecvPath.Data(), tobj, kTRUE);
            }
            return -1;
         }

         return -1;
      }

};


// ===================================================================================

// Timer used to invoke reply in the main process

class TReplyTimer : public TTimer {
   public:
      dabc::Command fCmd;

      TReplyTimer(dabc::Command cmd) :
         TTimer(0),
         fCmd(cmd)
      {
         Add();
      }

      Bool_t Notify() override
      {
         double res_tm = -1.;

         TGo4DabcAccess* acc = nullptr;

         if (!fCmd.null()) {

            acc = (TGo4DabcAccess*) fCmd.GetPtr("#DabcAccess");
            TGo4DabcProxy* prox = (TGo4DabcProxy*) fCmd.GetPtr("#DabcProxy");

            if (acc) {
               res_tm = acc->ProcessCommandReply(fCmd);
            } else
            if (prox) {
               prox->ReplyCommand(&fCmd);
               fCmd.Reply();
            }
         }

         if (res_tm<0)  {
            fCmd.Release();
            if (acc) delete acc;
            Remove();
            delete this;
            return kFALSE;
         }

         Start((Long_t)res_tm*1000, kTRUE);

         return kTRUE;
      }
};

class ReplyWorker : public dabc::Worker {
   protected:
      bool ReplyCommand(dabc::Command cmd) override
      {
         if (cmd.GetPtr("#DabcAccess") || cmd.GetPtr("#DabcProxy")) {
            // DOUT0("CreateReplTimer");
            new TReplyTimer(cmd);
            return false;
         }

         return dabc::Worker::ReplyCommand(cmd);
      }

   public:
      ReplyWorker(const std::string& name) :
         dabc::Worker(MakePair(name))
      {
      }

};



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

      Bool_t next() override
      {
         if (fParent.NumChilds() == 0) return kFALSE;

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

      Bool_t isfolder() override { return fChild.NumChilds() > 0; }

      Int_t getflag(const char *flagname) override
      {
         if (strcmp(flagname,"IsRemote") == 0) return 1;
         return -1;
      }

      TGo4LevelIter* subiterator() override
      {
         return fChild.NumChilds() > 0 ? new TGo4DabcLevelIter(fChild) : nullptr;
      }

      TGo4Slot *getslot() override { return nullptr; }

      const char *name() override { return fChild.GetName(); }
      const char *info() override { return "item from dabc"; }
      Int_t sizeinfo() override { return 0; }

      Int_t GetKind() override
      {
         if (isfolder()) return TGo4Access::kndFolder;

         if (IsRateHistory(fChild)) return TGo4Access::kndObject;

         fClNameBuf = GetRootClassName(fChild);

         if (fClNameBuf.Length()>0) return TGo4Access::kndObject;

         return -1;
      }

      const char *GetClassName() override
      {
         if (IsRateHistory(fChild)) return "TGraph";

         fClNameBuf = GetRootClassName(fChild);

         if (fClNameBuf.Length()>0) return fClNameBuf.Data();

         return "dabc::Hierarchy";
      }
};


// =====================================================================================


TGo4DabcProxy::TGo4DabcProxy() :
   TGo4ServerProxy(),
   fNodeName(),
   fxHierarchy(nullptr),
   fxParentSlot(nullptr)
{
}

TGo4DabcProxy::~TGo4DabcProxy()
{
   if (fxHierarchy) {
      delete ((dabc::Hierarchy*) fxHierarchy);
      fxHierarchy = nullptr;
   }
}

const char *TGo4DabcProxy::GetDabcVersion()
{
   return DABC_RELEASE;
}

Bool_t TGo4DabcProxy::Connect(const char *nodename)
{
   if (!dabc::CreateManager("cmd", 0)) return kFALSE;

   std::string node = dabc::MakeNodeName(nodename);

   if (!dabc::ConnectDabcNode(node)) {
      fNodeName = node;
      return kFALSE;
   }

   dabc::WorkerRef wrk = dabc::mgr.FindItem("/Go4ReplWrk");

   if (wrk.null()) {
      wrk = new ReplyWorker("/Go4ReplWrk");
      wrk()->AssignToThread(dabc::mgr.thread());
   }

   // printf("Connection to %s done\n", nodename);

   fNodeName = node;

   return UpdateHierarchy(kTRUE);
}

Bool_t TGo4DabcProxy::ReplyCommand(void* _cmd)
{
   dabc::Command cmd = *((dabc::Command*)_cmd);

   if (cmd.IsName(dabc::CmdGetNamesList::CmdName())) {

      dabc::Buffer buf = cmd.GetRawData();

      if (buf.null()) return kFALSE;

      if (!fxHierarchy)
         fxHierarchy = new dabc::Hierarchy();

      dabc::Hierarchy& hierarchy = *((dabc::Hierarchy*) fxHierarchy);
      hierarchy.Release();

      // DOUT0("Get raw data %p %u", buf.SegmentPtr(), buf.GetTotalSize());
      if (!hierarchy.ReadFromBuffer(buf)) return kFALSE;

      if (fxParentSlot)
         fxParentSlot->ForwardEvent(fxParentSlot, TGo4Slot::evObjUpdated);
   }

   //printf("Read from DABC node %s done\n", fNodeName.Data());
   return kTRUE;
}


Bool_t TGo4DabcProxy::UpdateHierarchy(Bool_t sync)
{
   if (fNodeName.IsNull()) return kFALSE;

   dabc::CmdGetNamesList cmd2;
   cmd2.SetReceiver(std::string(fNodeName.Data()) + dabc::Publisher::DfltName());
   cmd2.SetTimeout(10.);

   dabc::WorkerRef wrk = dabc::mgr.FindItem("/Go4ReplWrk");

   if (!sync && !wrk.null()) {
      cmd2.SetPtr("#DabcProxy", this);
      wrk()->Assign(cmd2);

      dabc::mgr.GetCommandChannel().Submit(cmd2);
      return kTRUE;
   }

   if (dabc::mgr.GetCommandChannel().Execute(cmd2)!=dabc::cmd_true) {
      TGo4Log::Error("Fail to get remote hierarchy");
      return kFALSE;
   }

   return ReplyCommand(&cmd2);
}

void TGo4DabcProxy::Initialize(TGo4Slot *slot)
{
   fxParentSlot = slot;
}

void TGo4DabcProxy::Finalize(TGo4Slot *slot)
{
}

Bool_t TGo4DabcProxy::HasSublevels() const
{
   if (!fxHierarchy) return kFALSE;

   dabc::Hierarchy& hierarchy = *((dabc::Hierarchy*) fxHierarchy);

   return hierarchy.NumChilds() > 0;
}

std::unique_ptr<TGo4Access> TGo4DabcProxy::ProvideAccess(const char *name)
{
   if (!fxHierarchy)
      return nullptr;

   dabc::Hierarchy& hierarchy = *((dabc::Hierarchy*) fxHierarchy);

   if (hierarchy.null())
      return nullptr;

   if (!name || !*name)
      return std::make_unique<TGo4DabcAccess>(fNodeName.Data(), hierarchy);

   dabc::Hierarchy child = hierarchy.FindChild(name);

   return child.null() ? nullptr : std::make_unique<TGo4DabcAccess>(fNodeName.Data(), child);
}

TGo4LevelIter* TGo4DabcProxy::MakeIter()
{
   if (!fxHierarchy) return nullptr;

   dabc::Hierarchy& hierarchy = *((dabc::Hierarchy*) fxHierarchy);

   return hierarchy.null() ? nullptr : new TGo4DabcLevelIter(hierarchy);
}

void TGo4DabcProxy::WriteData(TGo4Slot *slot, TDirectory* dir, Bool_t onlyobjs)
{
}

void TGo4DabcProxy::ReadData(TGo4Slot *slot, TDirectory* dir)
{
}

void TGo4DabcProxy::Update(TGo4Slot *slot, Bool_t strong)
{
   if (strong) {
      printf("GO4 WANTS update DABC hierarchy - do it\n");
      UpdateHierarchy(kFALSE);
   }
}

Bool_t TGo4DabcProxy::RefreshNamesList()
{
   return UpdateHierarchy(kFALSE);
}


#else

TGo4DabcProxy::TGo4DabcProxy() :
   TGo4ServerProxy(),
   fNodeName(),
   fxHierarchy(nullptr),
   fxParentSlot(nullptr)
{
}

TGo4DabcProxy::~TGo4DabcProxy()
{
}

const char *TGo4DabcProxy::GetDabcVersion()
{
   return nullptr;
}

Bool_t TGo4DabcProxy::Connect(const char *nodename)
{
   return kFALSE;
}

Bool_t TGo4DabcProxy::UpdateHierarchy(Bool_t sync)
{
   return kFALSE;
}

Bool_t TGo4DabcProxy::ReplyCommand(void* cmd)
{
   return kFALSE;
}


void TGo4DabcProxy::Initialize(TGo4Slot *slot)
{
}

void TGo4DabcProxy::Finalize(TGo4Slot *slot)
{
}

Bool_t TGo4DabcProxy::HasSublevels() const
{
   return kFALSE;
}

std::unique_ptr<TGo4Access> TGo4DabcProxy::ProvideAccess(const char *name)
{
   return nullptr;
}

TGo4LevelIter* TGo4DabcProxy::MakeIter()
{
   return nullptr;
}

void TGo4DabcProxy::WriteData(TGo4Slot *slot, TDirectory* dir, Bool_t onlyobjs)
{

}

void TGo4DabcProxy::ReadData(TGo4Slot *slot, TDirectory* dir)
{

}

void TGo4DabcProxy::Update(TGo4Slot *slot, Bool_t strong)
{
}

Bool_t TGo4DabcProxy::RefreshNamesList()
{
   return kFALSE;
}


#endif
