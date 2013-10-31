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
#include "TClass.h"
#include "TAxis.h"
#include "TTimer.h"
#include "TBufferFile.h"

#include "TGo4Log.h"

#include "dabc/Hierarchy.h"
#include "dabc/Manager.h"
#include "dabc/Publisher.h"
#include "dabc/Configuration.h"

extern "C" void R__unzip(Int_t *nin, UChar_t *bufin, Int_t *lout, char *bufout, Int_t *nout);
extern "C" int R__unzip_header(Int_t *nin, UChar_t *bufin, Int_t *lout);

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
      std::string      fNode;
      dabc::Hierarchy  fItem;
      std::string      fObjName;
      std::string      fItemName;
      std::string      fRootClassName;
      bool             fIsRate;
      int              fHistoryLength;
      TGo4ObjectReceiver* fxReceiver;
      TString             fxRecvPath;

   public:
      TGo4DabcAccess(const std::string& node, const dabc::Hierarchy& item) :
         TGo4Access(),
         fNode(node),
         fItem(item),
         fObjName(),
         fItemName(),
         fRootClassName(),
         fIsRate(false),
         fHistoryLength(0),
         fxReceiver(0),
         fxRecvPath()
      {
         fObjName = item.GetName();
         fItemName = item.ItemName();

         std::string kind = item.GetField("dabc:kind").AsStr();

         fHistoryLength = item.GetField("dabc:history").AsInt();

         if (kind.find("ROOT.") == 0) {
            kind.erase(0,5);
            fRootClassName = kind;
         } else
         if (kind=="rate") {
            fIsRate = true;
            if (fHistoryLength > 0) fRootClassName = "TGraph";
         }

         // printf("Create %s\n",fItem.GetName());
      }

      virtual ~TGo4DabcAccess() {}

      virtual Bool_t IsRemote() const { return kTRUE; }

      virtual Bool_t CanGetObject() const
      {
         return kFALSE;

         if (fIsRate && (fHistoryLength>0)) return kTRUE;
         if (!fRootClassName.empty()) return kTRUE;
         return kFALSE;
      }

/*

      Bool_t GetRateHistory(TObject* &obj, Bool_t &owner) const
      {
         dabc::CmdPublisherGet cmd2;
         cmd2.SetStr("Item", fItemName);
         cmd2.SetUInt("history", fHistoryLength);
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

         if (cnt==0) return kFALSE;

         TGraph* gr = new TGraph(cnt);
         gr->SetName(fObjName.c_str());
         gr->SetTitle(Form("%s ratemeter", fItemName.c_str()));
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


      virtual Bool_t GetObject(TObject* &obj, Bool_t &owner) const
      {
         if (fIsRate && (fHistoryLength>0))
            return GetRateHistory(obj, owner);

         // printf("GO4 WANT ITEM %s\n", fItem.ItemName().c_str());

         if (fRootClassName.empty()) return kFALSE;

         TClass *cl = TClass::GetClass(fRootClassName.c_str());
         if (!cl) {
            TGo4Log::Error("GetObject Unknown class %s", fRootClassName.c_str());
            return kFALSE;
         }

         if (!cl->InheritsFrom(TObject::Class())) {
           // in principle user should call TKey::ReadObjectAny!
            TGo4Log::Error("GetObject Class %s not derived from TObject", fRootClassName.c_str());
            return kFALSE;
         }

         dabc::CmdGetBinary cmd;
         cmd.SetStr("Item", fItemName);
         // cmd.SetUInt("version", version);
         cmd.SetTimeout(5);
         cmd.SetReceiver(fNode + dabc::Publisher::DfltName());

         if (dabc::mgr.GetCommandChannel().Execute(cmd)!=dabc::cmd_true) {
            printf("Fail to get item\n");
            return kFALSE;
         }

         dabc::Buffer buf = cmd.GetRawData();

         dabc::BinDataHeader* hdr = (dabc::BinDataHeader*) buf.SegmentPtr();

         // void* rawdata = hdr->rawdata();

         printf("GET RAW DATA of size %u\n", buf.GetTotalSize());


         char *pobj = (char*)cl->New();

         if (!pobj) {
            TGo4Log::Error("ReadObj - Cannot create new object of class %s", cl->GetName());
            return kFALSE;
         }

         Int_t baseOffset = cl->GetBaseClassOffset(TObject::Class());
         if (baseOffset==-1) {
            // cl does not inherit from TObject.
            // Since this is not possible yet, the only reason we could reach this code
            // is because something is screw up in the ROOT code.
            TGo4Log::Error("ReadObj Incorrect detection of the inheritance from TObject for class %s.",
                  cl->GetName());
         }

         TObject *tobj = (TObject*)(pobj+baseOffset);
           // Create an instance of this class

         char* rawbuf(0);
         Int_t rawbuflen(0);

         if (hdr->is_zipped()) {
            rawbuf = new char[hdr->zipped];
            rawbuflen = hdr->zipped;

            // target
            char *objbuf = rawbuf;

            // source
            UChar_t *bufcur = (UChar_t *) hdr->rawdata();

            Int_t nin, nout = 0, nbuf;
            Int_t noutot = 0;
            while (1) {
               Int_t hc = R__unzip_header(&nin, bufcur, &nbuf);
               if (hc!=0) break;
               R__unzip(&nin, bufcur, &nbuf, objbuf, &nout);
               if (!nout) break;
               noutot += nout;
               if (noutot >= rawbuflen) break;
               bufcur += nin;
               objbuf += nout;
            }
            // mark size of buffer 0
            if (nout==0) {
               rawbuflen = 0;
            }
         } else {
            rawbuf = (char*) hdr->rawdata();
            rawbuflen = hdr->payload;
         }

         if (rawbuflen>0) {
            TBufferFile buf(TBuffer::kRead, rawbuflen, rawbuf, kFALSE);
            buf.MapObject(pobj,cl);
            tobj->Streamer(buf);
         } else {
            cl->Destructor(pobj);
            pobj = 0;
            tobj = 0;
         }

         if (hdr->is_zipped())
            delete[] rawbuf;

         if (tobj) {
            obj = tobj;
            owner = kTRUE;
            return kTRUE;
         }

         return kFALSE;
      }
*/

      virtual Bool_t GetObject(TObject* &obj, Bool_t &owner) const
        { return kFALSE; }

      virtual TClass* GetObjectClass() const
      {
         // printf("Get class\n");
         if (fRootClassName.length() > 0)
            return (TClass*) gROOT->GetListOfClasses()->FindObject(fRootClassName.c_str());
         return 0;
      }

      virtual const char* GetObjectName() const
      {
         return fObjName.c_str();
      }

      virtual const char* GetObjectClassName() const
      {
         // printf("Get class name\n");

         if (fRootClassName.length()>0) return fRootClassName.c_str();

         return "dabc::Hierarchy";
      }


      virtual Int_t AssignObjectTo(TGo4ObjectReceiver* rcv, const char* path)
      {
         if (rcv==0) return 0;

         dabc::WorkerRef wrk = dabc::mgr.FindItem("/Go4ReplWrk");
         if (wrk.null()) return 0;

         if (fIsRate && (fHistoryLength>0)) {
            dabc::CmdPublisherGet cmd2;
            cmd2.SetStr("Item", fItemName);
            cmd2.SetUInt("history", fHistoryLength);
            cmd2.SetTimeout(5.);
            cmd2.SetReceiver(fNode + dabc::Publisher::DfltName());

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

            dabc::CmdGetBinary cmd;
            cmd.SetStr("Item", fItemName);
            // cmd.SetUInt("version", version);
            cmd.SetTimeout(5);
            cmd.SetReceiver(fNode + dabc::Publisher::DfltName());

            cmd.SetPtr("#DabcAccess", this);
            fxReceiver = rcv;
            fxRecvPath = path;
            wrk()->Assign(cmd);

            if (dabc::mgr.GetCommandChannel().Submit(cmd)) return 2;
         }


         return 0;
      }

      bool ReplyCommand(dabc::Command cmd)
      {

         if (fIsRate && (fHistoryLength>0)) {
            dabc::Hierarchy res;
            res.Create("get");
            res.SetVersion(cmd.GetUInt("version"));
            res.ReadFromBuffer(cmd.GetRawData());

            //DOUT0("GET:%s RES = \n%s", fItem.ItemName().c_str(), res.SaveToXml(dabc::xmlmask_History).c_str());

            dabc::HistoryIter iter = res.MakeHistoryIter();
            int cnt = 0;
            while (iter.next()) cnt++;

            // DOUT0("ITERATOR cnt %d", cnt);

            if (cnt==0) return kFALSE;

            TGraph* gr = new TGraph(cnt);
            gr->SetName(fObjName.c_str());
            gr->SetTitle(Form("%s ratemeter", fItemName.c_str()));
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

            DoObjectAssignement(fxReceiver, fxRecvPath.Data(), gr, kTRUE);
            return true;
         }

         if (!fRootClassName.empty()) {
            TClass *cl = TClass::GetClass(fRootClassName.c_str());

            dabc::Buffer buf = cmd.GetRawData();

            dabc::BinDataHeader* hdr = (dabc::BinDataHeader*) buf.SegmentPtr();

            // printf("GET RAW DATA of size %u\n", buf.GetTotalSize());

            char *pobj = (char*)cl->New();

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
               return true;
            }

            TObject *tobj = (TObject*)(pobj+baseOffset);
              // Create an instance of this class

            char* rawbuf(0);
            Int_t rawbuflen(0);

            if (hdr->is_zipped()) {
               rawbuf = new char[hdr->zipped];
               rawbuflen = hdr->zipped;

               // target
               char *objbuf = rawbuf;

               // source
               UChar_t *bufcur = (UChar_t *) hdr->rawdata();

               Int_t nin, nout = 0, nbuf;
               Int_t noutot = 0;
               while (1) {
                  Int_t hc = R__unzip_header(&nin, bufcur, &nbuf);
                  if (hc!=0) break;
                  R__unzip(&nin, bufcur, &nbuf, objbuf, &nout);
                  if (!nout) break;
                  noutot += nout;
                  if (noutot >= rawbuflen) break;
                  bufcur += nin;
                  objbuf += nout;
               }
               // mark size of buffer 0
               if (nout==0) {
                  rawbuflen = 0;
               }
            } else {
               rawbuf = (char*) hdr->rawdata();
               rawbuflen = hdr->payload;
            }

            if (rawbuflen>0) {
               TBufferFile buf(TBuffer::kRead, rawbuflen, rawbuf, kFALSE);
               buf.MapObject(pobj,cl);
               tobj->Streamer(buf);
            } else {
               cl->Destructor(pobj);
               pobj = 0;
               tobj = 0;
            }

            if (hdr->is_zipped())
               delete[] rawbuf;

            if (tobj) DoObjectAssignement(fxReceiver, fxRecvPath.Data(), tobj, kTRUE);
            return true;
         }

         return true;
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

      virtual Bool_t Notify()
      {
         Remove();

         if (!fCmd.null()) {
            TGo4DabcAccess* acc = (TGo4DabcAccess*) fCmd.GetPtr("#DabcAccess");
            if (acc) acc->ReplyCommand(fCmd);
            fCmd.Reply();
         }

         delete this;

         return kFALSE;
      }
};

class ReplyWorker : public dabc::Worker {
   protected:
      virtual bool ReplyCommand(dabc::Command cmd)
      {
         if (cmd.GetPtr("#DabcAccess") != 0) {
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

      virtual Int_t getflag(const char* flagname)
      {
         if (strcmp(flagname,"IsRemote")==0) return 1;
         return -1;
      }

      virtual TGo4LevelIter* subiterator()
      {
         return fChild.NumChilds() > 0 ? new TGo4DabcLevelIter(fChild) : 0;
      }

      virtual TGo4Slot* getslot() { return 0; }

      virtual const char* name() { return fChild.GetName(); }
      virtual const char* info() { return "item from dabc"; }
      virtual Int_t sizeinfo() { return 0; }

      virtual Int_t GetKind() {
         if (isfolder()) return TGo4Access::kndFolder;

         if (IsRateHistory(fChild)) return TGo4Access::kndObject;

         fClNameBuf = GetRootClassName(fChild);

         if (fClNameBuf.Length()>0) return TGo4Access::kndObject;

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


   dabc::WorkerRef wrk = dabc::mgr.FindItem("/Go4ReplWrk");

   if (wrk.null()) {
      wrk = new ReplyWorker("/Go4ReplWrk");
      wrk()->AssignToThread(dabc::mgr.thread());
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
      return new TGo4DabcAccess(fNodeName.Data(), hierarchy);
   }

   dabc::Hierarchy child = hierarchy.FindChild(name);

   //printf("Create access to child %s ptr %p\n", name, child());

   return child.null() ? 0 : new TGo4DabcAccess(fNodeName.Data(), child);
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
