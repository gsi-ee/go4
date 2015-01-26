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

#include "TGo4HttpProxy.h"

#include "TThread.h"
#include "TROOT.h"
#include "TClass.h"
#include "TList.h"
#include "TGo4Slot.h"
#include "TBufferFile.h"

#include <QtNetwork>

void QHttpProxy::httpFinished()
{
   QByteArray res = fReply->readAll();
   fReply->deleteLater();
   fReply = 0;

   fProxy->GetReply(res);
}


void QHttpProxy::StartRequest(const char* url)
{
   fReply = qnam.get(QNetworkRequest(QUrl(url)));
   connect(fReply, SIGNAL(finished()),
         this, SLOT(httpFinished()));
}


const char* GetHttpRootClassName(const char* kind)
{
   if ((kind==0) || (*kind==0)) return 0;
   if (strncmp(kind,"ROOT.", 5)!=0) return 0;
   return kind+5;
}

// ============================================================================================

TGo4HttpAccess::TGo4HttpAccess(TGo4HttpProxy* proxy, XMLNodePointer_t node, const char* path) :
   TGo4Access(),
   fProxy(proxy),
   fNode(node),
   fPath(path),
   fReceiver(0),
   fRecvPath(),
   fReply(0)
{
}

TClass* TGo4HttpAccess::GetObjectClass() const
{
   const char* clname = GetHttpRootClassName(fProxy->fXML->GetAttr(fNode,"_kind"));
   if (clname!=0) return (TClass*) gROOT->GetListOfClasses()->FindObject(clname);
   return 0;
}

const char* TGo4HttpAccess::GetObjectName() const
{
   return fProxy->fXML->GetAttr(fNode,"_name");
}

const char* TGo4HttpAccess::GetObjectClassName() const
{
   const char* clname = GetHttpRootClassName(fProxy->fXML->GetAttr(fNode,"_kind"));

   return clname ? clname : "TObject";
}


Int_t TGo4HttpAccess::AssignObjectTo(TGo4ObjectManager* rcv, const char* path)
{
   if (rcv==0) return 0;

   TClass* obj_cl = GetObjectClass();
   if (obj_cl==0) {
      printf("TGo4HttpAccess fail to get object class %s\n", GetObjectClassName());
      return 0;
   }

   if (obj_cl->GetBaseClassOffset(TObject::Class()) != 0) {
      printf("TGo4HttpAccess cannot used class %s not derived from TObject\n", GetObjectClassName());
      return 0;
   }

   fReceiver = rcv;
   fRecvPath = path;

   TString url = fProxy->fNodeName;
   if (fPath.Length()>0) { url.Append("/"); url.Append(fPath); }
   url.Append("/root.bin");

   printf("Request URL %s\n", url.Data());

   fReply = fProxy->fComm.qnam.get(QNetworkRequest(QUrl(url.Data())));
   connect(fReply, SIGNAL(finished()), this, SLOT(httpFinished()));

   return 2;
}

void TGo4HttpAccess::httpFinished()
{
   QByteArray res = fReply->readAll();
   fReply->deleteLater();
   fReply = 0;

   TClass* obj_cl = GetObjectClass();
   if ((obj_cl==0) || (obj_cl->GetBaseClassOffset(TObject::Class()) != 0)) return;

   TObject* obj = (TObject*) obj_cl->New();
   if (obj==0) {
      printf("TGo4HttpAccess fail to create object of class %s\n", GetObjectClassName());
      return;
   }

   TBufferFile buf(TBuffer::kRead, res.size(), res.data(), kFALSE);
   buf.MapObject(obj);

   obj->Streamer(buf);

   DoObjectAssignement(fReceiver, fRecvPath.Data(), obj, kTRUE);
}

// =========================================================================

class TGo4HttpLevelIter : public TGo4LevelIter {
   protected:
      TXMLEngine      *fXML;
      XMLNodePointer_t fParent;
      XMLNodePointer_t fChild;
      unsigned fCnt;

   public:
      TGo4HttpLevelIter(TXMLEngine* xml, XMLNodePointer_t item) :
         TGo4LevelIter(),
         fXML(xml),
         fParent(item),
         fChild(),
         fCnt(0)
         {
         }

      virtual ~TGo4HttpLevelIter() {}

      virtual Bool_t next()
      {
         if (fParent == 0) return kFALSE;

         if (fChild == 0) {
            fCnt = 0;
            fChild = fXML->GetChild(fParent);
         } else {
            fCnt++;
            fChild = fXML->GetNext(fChild);
         }

         return fChild!=0;
      }

      virtual Bool_t isfolder() { return (fChild!=0) && (fXML->GetChild(fChild)!=0); }

      virtual Int_t getflag(const char* flagname)
      {
         if (strcmp(flagname,"IsRemote")==0) return 1;
         return -1;
      }

      virtual TGo4LevelIter* subiterator()
      {
         if (!isfolder()) return 0;
         return new TGo4HttpLevelIter(fXML,fChild);
      }

      virtual TGo4Slot* getslot() { return 0; }

      virtual const char* name() { return fXML->GetAttr(fChild,"_name"); }
      virtual const char* info() { return fXML->GetAttr(fChild,"_title"); }
      virtual Int_t sizeinfo() { return 0; }

      virtual Int_t GetKind() {
         if (isfolder()) return TGo4Access::kndFolder;

         if (GetHttpRootClassName(fXML->GetAttr(fChild,"_kind"))!=0) return TGo4Access::kndObject;

         return -1;
      }

      virtual const char* GetClassName()
      {
         const char* res = GetHttpRootClassName(fXML->GetAttr(fChild,"_kind"));

         if (res!=0) return res;

         return isfolder() ? "TFolder" : "TObject";
      }
};



// =====================================================================

TGo4HttpProxy::TGo4HttpProxy() :
   TGo4ServerProxy(),
   fNodeName(),
   fXML(0),
   fxHierarchy(0),
   fxParentSlot(0),
   fComm(this)
{
   fXML = new TXMLEngine;
}

TGo4HttpProxy::~TGo4HttpProxy()
{
   fXML->FreeDoc(fxHierarchy);
   fxHierarchy = 0;

   delete fXML; fXML = 0;
}

XMLNodePointer_t TGo4HttpProxy::FindItem(XMLNodePointer_t curr, const char* name)
{
   if ((curr==0) || (name==0) || (*name==0)) return curr;

   const char* slash = strchr(name,'\/');
   bool doagain = false;

   do {
      int len = (slash==0) ? strlen(name) : (slash - name);

      XMLNodePointer_t chld = fXML->GetChild(curr);
      while (chld!=0) {
         const char* _name = fXML->GetAttr(chld,"_name");

         if ((_name!=0) && (strncmp(_name, name, len)==0))
            return FindItem(chld, slash ? slash+1 : 0);

         chld = fXML->GetNext(chld);
      }

      // we try to process situation when item name contains slashes
      doagain = slash!=0;
      if (slash) slash = strchr(slash+1,'\/');

   } while (doagain);

   return 0;

}


void TGo4HttpProxy::GetReply(QByteArray& res)
{
   // printf("THRD:%ld HTTP FINISHED len = %d res = \n%s\n", TThread::SelfId(), res.size(), res.data());

   if (res.size()>0) {
      XMLDocPointer_t doc = fXML->ParseString(res.data());
      if (doc!=0) {
         fXML->FreeDoc(fxHierarchy);
         fxHierarchy = doc;
      }

      if (fxParentSlot!=0)
        fxParentSlot->ForwardEvent(fxParentSlot, TGo4Slot::evObjAssigned);
   }
}

Bool_t TGo4HttpProxy::Connect(const char* nodename)
{
   fNodeName = nodename;

   printf("THRD:%ld Connect %s IsA %s \n", TThread::SelfId(), nodename, ClassName());

   return UpdateHierarchy(kTRUE);
}

Bool_t TGo4HttpProxy::UpdateHierarchy(Bool_t sync)
{
   TString req = fNodeName + "/h.xml?compact&generic";

   fComm.StartRequest(req.Data());

   return kTRUE;
}

void TGo4HttpProxy::Initialize(TGo4Slot* slot)
{
   fxParentSlot = slot;
}

void TGo4HttpProxy::Finalize(TGo4Slot* slot)
{
}

Bool_t TGo4HttpProxy::HasSublevels() const
{
   return fxHierarchy != 0;
}

TGo4Access* TGo4HttpProxy::MakeProxy(const char* name)
{
   if (fxHierarchy == 0) return 0;

   XMLNodePointer_t top = fXML->GetChild(fXML->DocGetRootElement(fxHierarchy));

   XMLNodePointer_t item = FindItem(top, name);
   if (item==0) return 0;

   return new TGo4HttpAccess(this, item, name);
}

TGo4LevelIter* TGo4HttpProxy::MakeIter()
{
   if (fxHierarchy == 0) return 0;
   XMLNodePointer_t top = fXML->GetChild(fXML->DocGetRootElement(fxHierarchy));

   return top==0 ? 0 : new TGo4HttpLevelIter(fXML, top);
}

void TGo4HttpProxy::WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs)
{
}

void TGo4HttpProxy::ReadData(TGo4Slot* slot, TDirectory* dir)
{
}

void TGo4HttpProxy::Update(TGo4Slot* slot, Bool_t strong)
{
   if (strong) {
      UpdateHierarchy(kFALSE);
   }
}

Bool_t TGo4HttpProxy::RefreshNamesList()
{
   return UpdateHierarchy(kFALSE);
}

