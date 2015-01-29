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

#include "TROOT.h"
#include "TClass.h"
#include "TList.h"
#include "TH1.h"
#include "TH2.h"
#include "TGo4Slot.h"
#include "TBufferFile.h"

#include <QtNetwork>
#include <QTime>
#include <QApplication>

void QHttpProxy::httpFinished()
{
   QByteArray res = fReply->readAll();
   fReply->deleteLater();
   fReply = 0;
   fProxy->GetReply(res);
}

void QHttpProxy::httpError(QNetworkReply::NetworkError code)
{
   printf("QHttpProxy::httpError %d %s\n", code, fReply->errorString().toLatin1().constData());
}

void QHttpProxy::httpSslErrors (const QList<QSslError> & errors)
{
   // printf("QHttpProxy::httpSslErrors\n");
   if (fReply) fReply->ignoreSslErrors();
}

void QHttpProxy::StartRequest(const char* url)
{
   fReply = qnam.get(QNetworkRequest(QUrl(url)));

   connect(fReply, SIGNAL(finished()),
         this, SLOT(httpFinished()));

   connect(fReply, SIGNAL(error(QNetworkReply::NetworkError)),
         this, SLOT(httpError(QNetworkReply::NetworkError)));
   connect(fReply, SIGNAL(sslErrors(const QList<QSslError>&)),
         this, SLOT(httpSslErrors(const QList<QSslError>&)));

   QSslConfiguration cfg = fReply->sslConfiguration();
   cfg.setProtocol(QSsl::AnyProtocol/*QSsl::TlsV1SslV3*/);
   fReply->setSslConfiguration(cfg);
}


const char* GetHttpRootClassName(const char* kind)
{
   if ((kind==0) || (*kind==0)) return 0;
   if (strncmp(kind,"ROOT.", 5)!=0) return 0;
   return kind+5;
}

// ============================================================================================

TGo4HttpAccess::TGo4HttpAccess(TGo4HttpProxy* proxy, XMLNodePointer_t node, const char* path, Int_t kind) :
   TGo4Access(),
   fProxy(proxy),
   fNode(node),
   fPath(path),
   fKind(kind),
   fNameAttr(),
   fKindAttr(),
   fReceiver(0),
   fRecvPath(),
   fReply(0)
{
   const char* _name = fProxy->fXML->GetAttr(fNode,"_name");
   if (_name) fNameAttr = _name;
   const char* _kind = fProxy->fXML->GetAttr(fNode,"_kind");
   if (_kind) fKindAttr = _kind;
}

TClass* TGo4HttpAccess::GetObjectClass() const
{
   const char* clname = GetHttpRootClassName(fKindAttr.Data());
   if (clname!=0) return (TClass*) gROOT->GetListOfClasses()->FindObject(clname);
   return 0;
}

const char* TGo4HttpAccess::GetObjectName() const
{
   return fNameAttr.Data();
}

const char* TGo4HttpAccess::GetObjectClassName() const
{
   const char* clname = GetHttpRootClassName(fKindAttr.Data());

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

   switch (fKind) {
      case 0: url.Append("/h.xml?compact"); break;
      case 1: url.Append("/root.bin.gz"); break;
      case 2: url.Append("/get.xml"); break;
      default: url.Append("/root.bin.gz"); break;
   }

   // printf("Send request URL %s\n", url.Data());

   fReply = fProxy->fComm.qnam.get(QNetworkRequest(QUrl(url.Data())));
   connect(fReply, SIGNAL(finished()), this, SLOT(httpFinished()));

   return 2;
}

void TGo4HttpAccess::httpFinished()
{
   QByteArray res = fReply->readAll();
   fReply->deleteLater();
   fReply = 0;

   // printf("Get reply size %d\n", res.size());

   // do nothing
   if (res.size()==0) return;

   if (fKind == 0) {

      TXMLEngine* xml = fProxy->fXML;

      XMLDocPointer_t doc = xml->ParseString(res.data());

      if (doc==0) return;

      XMLNodePointer_t top = xml->GetChild(xml->DocGetRootElement(doc));

      xml->FreeAllAttr(fNode);

      XMLAttrPointer_t attr = xml->GetFirstAttr(top);
      while (attr!=0) {
         xml->NewAttr(fNode, 0, xml->GetAttrName(attr), xml->GetAttrValue(attr));
         attr = xml->GetNextAttr(attr);
      }

      XMLNodePointer_t chld;
      while ((chld = xml->GetChild(top)) != 0) {
         xml->UnlinkNode(chld);
         xml->AddChild(fNode, chld);
      }

      xml->FreeDoc(doc);

      if (fProxy->fxParentSlot!=0)
         fProxy->fxParentSlot->ForwardEvent(fProxy->fxParentSlot, TGo4Slot::evObjAssigned);

      return;
   }

   if (fKind == 2) {
      TXMLEngine* xml = fProxy->fXML;

      XMLDocPointer_t doc = xml->ParseString(res.data());
      if (doc==0) return;

      XMLNodePointer_t top = xml->DocGetRootElement(doc);

      const char* _kind = xml->GetAttr(top, "_kind");
      const char* _name = xml->GetAttr(top, "_name");
      const char* _title = xml->GetAttr(top, "_title");

      TObject* obj = 0;

      if (strcmp(_kind,"ROOT.TH1D")==0) {
         Int_t nbins = xml->GetIntAttr(top, "nbins");
         Int_t left = TString(xml->GetAttr(top, "left")).Atof();
         Int_t right = TString(xml->GetAttr(top, "right")).Atof();
         TH1D* h1 = new TH1D(_name, _title, nbins, left, right);
         h1->SetDirectory(0);
         const char* bins = xml->GetAttr(top, "bins") + 1;
         for (int n =-3; n<nbins; n++) {
            const char* separ = strpbrk(bins,",]");
            if (separ==0) { printf("Error parsing histogram bins\n"); break; }
            if (n>=0) {
               Double_t v = TString(bins, separ-bins).Atof();
               h1->SetBinContent(n+1, v);
            }
            bins = separ+1;
         }
         obj = h1;
      } else {
         Int_t nbins1 = xml->GetIntAttr(top, "nbins1");
         Int_t left1 = TString(xml->GetAttr(top, "left1")).Atof();
         Int_t right1 = TString(xml->GetAttr(top, "right1")).Atof();
         Int_t nbins2 = xml->GetIntAttr(top, "nbins2");
         Int_t left2 = TString(xml->GetAttr(top, "left2")).Atof();
         Int_t right2 = TString(xml->GetAttr(top, "right2")).Atof();
         TH2D* h2 = new TH2D(_name, _title, nbins1, left1, right1, nbins2, left2, right2);
         h2->SetDirectory(0);
         const char* bins = xml->GetAttr(top, "bins") + 1;
         for (int n =-6; n<nbins1*nbins2; n++) {
            const char* separ = strpbrk(bins,",]");
            if (separ==0) { printf("Error parsing histogram bins\n"); break; }
            if (n>=0) {
               Double_t v = TString(bins, separ-bins).Atof();
               h2->SetBinContent(n % nbins1 + 1, n / nbins1 + 1,  v);
            }
            bins = separ+1;
         }
         obj = h2;
      }

      xml->FreeDoc(doc);

      DoObjectAssignement(fReceiver, fRecvPath.Data(), obj, kTRUE);
      return;
   }


   TClass* obj_cl = GetObjectClass();
   if ((obj_cl==0) || (obj_cl->GetBaseClassOffset(TObject::Class()) != 0)) return;

   TObject* obj = (TObject*) obj_cl->New();
   if (obj==0) {
      printf("TGo4HttpAccess fail to create object of class %s\n", GetObjectClassName());
      return;
   }

   void* res_buf =  res.data();
   int res_size = res.size();

   TBufferFile buf(TBuffer::kRead, res_size, res_buf, kFALSE);
   buf.MapObject(obj, obj_cl);

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

         // filter-out streamer info item
         if (fChild!=0) {
            if (strcmp("StreamerInfo", name())==0)
               if (strcmp("TStreamerInfoList", GetClassName())==0) return next();
         }

         return fChild!=0;
      }

      virtual Bool_t isfolder() {
         return fXML->GetChild(fChild)!=0;
      }

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

         if (fXML->HasAttr(fChild,"_more")) return TGo4Access::kndMoreFolder;

         const char* drawfunc = fXML->GetAttr(fChild,"_drawfunc");
         if ((drawfunc!=0) && !strcmp(drawfunc, "GO4.drawParameter")) return TGo4Access::kndGo4Param;

         const char* clname = GetHttpRootClassName(fXML->GetAttr(fChild,"_kind"));

         return clname==0 ? TGo4Access::kndNone : TGo4Access::kndObject;
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
   // printf("Get reply %d\n", res.size());

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

   if ((fNodeName.Index("http://")!=0)  && (fNodeName.Index("https://")!=0))
      fNodeName = TString("http://") + fNodeName;

   return UpdateHierarchy(kTRUE);
}

Bool_t TGo4HttpProxy::UpdateHierarchy(Bool_t sync)
{
   if (fComm.fReply!=0) return kTRUE;

   TString req = fNodeName + "/h.xml?compact";

   // printf("Send request %s\n", req.Data());

   fComm.StartRequest(req.Data());

   if (sync) {
      QTime t;
      t.start();

      // wait two seconds -
      while (t.elapsed()<2000) {
         if (fComm.fReply==0) break;
         QApplication::processEvents();
      }
   }

   return kTRUE;
}

Bool_t TGo4HttpProxy::HasSublevels() const
{
   return fxHierarchy != 0;
}

TGo4Access* TGo4HttpProxy::ProvideAccess(const char* name)
{
   if (fxHierarchy == 0) return 0;

   XMLNodePointer_t top = fXML->GetChild(fXML->DocGetRootElement(fxHierarchy));

   XMLNodePointer_t item = FindItem(top, name);
   if (item==0) return 0;

   Int_t kind = 1;

   if (fXML->HasAttr(item,"_dabc_hist")) kind = 2; else
   if (fXML->HasAttr(item,"_more")) kind = 0;

   return new TGo4HttpAccess(this, item, name, kind);
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

