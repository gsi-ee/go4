// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4HttpProxy.h"

#include <string.h>

#include "TROOT.h"
#include "TClass.h"
#include "TList.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TBufferFile.h"

#include "TGo4Condition.h"
#include "TGo4Slot.h"
#include "TGo4ObjectProxy.h"
#include "TGo4ObjectManager.h"
#include "TGo4Ratemeter.h"
#include "TGo4AnalysisStatus.h"
#include "TGo4HistogramStatus.h"

#include <QtNetwork>
#include <QTime>
#include <QApplication>
#include <QEventLoop>
#include <QInputDialog>


QHttpProxy::QHttpProxy(TGo4HttpProxy* p) :
   QObject(),
   qnam(),
   fHReply(0),
   fProxy(p)
{
   connect(&qnam, SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
           this, SLOT(authenticationRequiredSlot(QNetworkReply*, QAuthenticator*)));
}

QHttpProxy::~QHttpProxy()
{
}

void QHttpProxy::httpFinished()
{
   if (fHReply==0) return;

   QByteArray res = fHReply->readAll();
   fHReply->deleteLater();
   fHReply = 0;
   fProxy->GetHReply(res);
}

void QHttpProxy::httpHReqError(QNetworkReply::NetworkError code)
{
   if (gDebug>0)
      printf("QHttpProxy::httpHReqError %d %s\n", code, fHReply ? fHReply->errorString().toLatin1().constData() : "---");
   if (fHReply) {
      fHReply->deleteLater();
      fHReply = 0;
   }
}

void QHttpProxy::updateRatemeter()
{
   if (fProxy) fProxy->ProcessUpdateTimer();
   QTimer::singleShot(2000, this, SLOT(updateRatemeter()));
}

void QHttpProxy::updateHierarchy()
{
   if (fProxy) fProxy->UpdateHierarchy(kFALSE);
}

void QHttpProxy::regularRequestFinished()
{
   if (fProxy) fProxy->ProcessRegularMultiRequest(kTRUE);
}


void QHttpProxy::StartRequest(const char* url)
{
   fHReply = qnam.get(QNetworkRequest(QUrl(url)));

   connect(fHReply, SIGNAL(finished()),
         this, SLOT(httpFinished()));

   connect(fHReply, SIGNAL(error(QNetworkReply::NetworkError)),
         this, SLOT(httpHReqError(QNetworkReply::NetworkError)));

   connect(fHReply, SIGNAL(sslErrors(const QList<QSslError>&)),
           fHReply, SLOT(ignoreSslErrors()));

   QSslConfiguration cfg = fHReply->sslConfiguration();
   cfg.setProtocol(QSsl::AnyProtocol/*QSsl::TlsV1SslV3*/);
   fHReply->setSslConfiguration(cfg);
}

void QHttpProxy::authenticationRequiredSlot(QNetworkReply* repl, QAuthenticator* auth)
{
   if (fProxy->fUserName.Length()>0) {
      auth->setUser(fProxy->fUserName.Data());
      auth->setPassword(fProxy->fPassword.Data());
      return;
   }

   bool ok = false;
   QString user_name =
         QInputDialog::getText(0, tr("Authentication required"),
                              tr("User name:"), QLineEdit::Normal,
                              "", &ok);
   if (!ok) return;
   QString passwd =
        QInputDialog::getText(0, tr("Authentication required"),
                              tr("User password:"), QLineEdit::Password,
                              "", &ok);
   if (!ok) return;

   fProxy->fUserName = user_name.toLatin1().constData();
   fProxy->fPassword = passwd.toLatin1().constData();

   auth->setUser(user_name);
   auth->setPassword(passwd);
}

const char* GetHttpRootClassName(const char* kind)
{
   if ((kind==0) || (*kind==0)) return 0;
   if (strncmp(kind,"ROOT.", 5)!=0) return 0;
   if (strcmp(kind+5,"TGo4AnalysisWebStatus")==0) return "TGo4AnalysisStatus";
   return kind+5;
}

// ============================================================================================

TGo4HttpAccess::TGo4HttpAccess(TGo4HttpProxy* proxy, XMLNodePointer_t node, Int_t kind, const char* extra_arg) :
   TGo4Access(),
   fProxy(proxy),
   fNode(node),
   fUrlPath(),
   fKind(kind),
   fNameAttr(),
   fKindAttr(),
   fExtraArg(),
   fReceiver(0),
   fRecvPath(),
   fReply(0)
{
   const char* _name = fProxy->fXML->GetAttr(fNode,"_realname");
   if (_name == 0) _name = fProxy->fXML->GetAttr(fNode,"_name");
   if (_name) fNameAttr = _name;

   fUrlPath = fProxy->MakeUrlPath(node);

   const char* _kind = fProxy->fXML->GetAttr(fNode,"_kind");
   if (_kind) fKindAttr = _kind;
   if (extra_arg!=0) fExtraArg = extra_arg;
}

TClass* TGo4HttpAccess::GetObjectClass() const
{
   if (fKind==3) return TGraph::Class();
   if (fKind==4) return gROOT->GetClass("TGo4ParameterStatus");
   if (fKind==5) return gROOT->GetClass("TTree");
   if (fKind==6) return gROOT->GetClass("TGo4AnalysisStatus");
   if (fKind==7) return gROOT->GetClass("TList");
   if (fKind==8) return gROOT->GetClass("TGo4HistogramStatus");
   return TGo4Proxy::GetClass(GetHttpRootClassName(fKindAttr.Data()));
}

const char* TGo4HttpAccess::GetObjectName() const
{
   return fNameAttr.Data();
}

const char* TGo4HttpAccess::GetObjectClassName() const
{
   if (fKind==3) return "TGraph";
   if (fKind==4) return "TGo4ParameterStatus";
   if (fKind==6) return "TGo4AnalysisStatus";
   if (fKind==7) return "TList";
   if (fKind==8) return "TGo4HistogramStatus";

   const char* clname = GetHttpRootClassName(fKindAttr.Data());

   return clname ? clname : "TObject";
}

Int_t TGo4HttpAccess::AssignObjectTo(TGo4ObjectManager* rcv, const char* path)
{
   if (rcv==0) return 0;

   TClass* obj_cl = GetObjectClass();
   if (obj_cl==0) {
      printf("TGo4HttpAccess fail to get class %s for object %s\n", GetObjectClassName(), path);
      return 0;
   }

   if (obj_cl->GetBaseClassOffset(TObject::Class()) != 0) {
      printf("TGo4HttpAccess cannot used class %s not derived from TObject\n", GetObjectClassName());
      return 0;
   }

   fReceiver = rcv;
   fRecvPath = path;

   TString url = fProxy->fNodeName;
   if (fUrlPath.Length()>0) { url.Append("/"); url.Append(fUrlPath); }

   switch (fKind) {
      case 0: url.Append("/h.xml?compact"); break;
      case 1: url.Append("/root.bin.gz"); break;
      case 2: url.Append("/get.xml"); break;
      case 3: url.Append("/get.xml.gz?history=100&compact"); break;
      case 4: url.Append("/exe.bin.gz?method=CreateStatus&_destroy_result_"); break;
      case 5: url.Append("/exe.bin.gz?method=CreateSampleTree&sample=0&_destroy_result_"); break;
      case 6: url.Append("/exe.bin.gz?method=CreateStatus&_destroy_result_"); break;
      case 7: url.Append("/exe.bin.gz?method=Select&max=10000"); break;
      case 8: url.Form("%s/Control/go4_sniffer/exe.bin.gz?method=CreateItemStatus&_destroy_result_&itemname=\"%s\"", fProxy->fNodeName.Data(), fUrlPath.Data()); break;
      default: url.Append("/root.bin.gz"); break;
   }

   if (fExtraArg.Length()>0) {
      if (url.Index("?") != kNPOS) url.Append("&"); else url.Append("?");
      url.Append(fExtraArg);
   }

   fReply = fProxy->fComm.qnam.get(QNetworkRequest(QUrl(url.Data())));
   connect(fReply, SIGNAL(finished()), this, SLOT(httpFinished()));

   connect(fReply, SIGNAL(error(QNetworkReply::NetworkError)),
           this, SLOT(httpError(QNetworkReply::NetworkError)));

   if (gDebug>2) printf("TGo4HttpAccess::AssignObjectTo Request URL %s\n", url.Data());

   return 2;
}

void TGo4HttpAccess::httpError(QNetworkReply::NetworkError)
{
   // may do special handling for http errors
}


void TGo4HttpAccess::httpFinished()
{
   QByteArray res = fReply->readAll();
   fReply->deleteLater();
   fReply = 0;

   if (gDebug>2) printf("TGo4HttpAccess::httpFinished Get reply size %d\n", res.size());

   // regular ratemeter update used to check connection status
   if (fUrlPath == "Status/Ratemeter") {
      Bool_t conn = res.size() > 0;

      // if proxy in shutdown state - cancel all action in case of communication error
      if (!conn && fProxy->CheckShutdown()) return;

      if (!conn) DoObjectAssignement(fReceiver, fRecvPath.Data(), new TNamed("disconnected","title"), kTRUE); else
      if (fProxy->fConnected != conn) fProxy->UpdateHierarchy(kFALSE);

      fProxy->fConnected = conn;
   }

   // do nothing when nothing received
   if (res.size()==0) { if (gDebug>0) printf("TGo4HttpAccess::httpFinished error with %s\n", fUrlPath.Data()); return; }

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
   }

   TObject* obj = 0;

   if (fKind == 2) {
      TXMLEngine* xml = fProxy->fXML;

      XMLDocPointer_t doc = xml->ParseString(res.data());
      if (doc==0) return;

      XMLNodePointer_t top = xml->DocGetRootElement(doc);

      const char* _kind = xml->GetAttr(top, "_kind");
      const char* _name = xml->GetAttr(top, "_name");
      const char* _title = xml->GetAttr(top, "_title");

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

   } else

   if (fKind == 3) {
      TXMLEngine* xml = fProxy->fXML;

      // printf("History %s\n", res.data());

      XMLDocPointer_t doc = xml->ParseString(res.data());
      if (doc==0) return;

      XMLNodePointer_t top = xml->DocGetRootElement(doc);

      XMLNodePointer_t chld = top;
      Int_t cnt = 0;
      while (chld!=0) {
         if (xml->GetAttr(chld, "value") && xml->GetAttr(chld, "time")) cnt++;
         chld = (chld==top) ? xml->GetChild(top) : xml->GetNext(chld);
      }

      TGraph* gr = new TGraph(cnt);
      gr->SetName(xml->GetAttr(top, "_name"));
      gr->SetTitle(Form("%s ratemeter", xml->GetAttr(top, "_name")));

      chld = top;
      Int_t i = cnt-1;
      while (chld!=0) {
         const char* time = xml->GetAttr(chld, "time");
         const char* value = xml->GetAttr(chld, "value");
         if ((time!=0) && (value!=0)) {
            QDateTime tm = QDateTime::fromString(time, Qt::ISODate);
            gr->SetPoint(i, tm.toTime_t(), TString(value).Atof());
            i = (i+1) % cnt;
         }
         chld = (chld==top) ? xml->GetChild(top) : xml->GetNext(chld);
      }

      xml->FreeDoc(doc);
      obj = gr;

      gr->GetXaxis()->SetTimeDisplay(1);
      gr->GetXaxis()->SetTimeFormat("%H:%M:%S%F1970-01-01 00:00:00");
   } else {

      TClass* obj_cl = GetObjectClass();

      if (gDebug>2) printf("TGo4HttpAccess::httpFinished Reconstruct object class %s\n", obj_cl ? obj_cl->GetName() : "---");

      if ((obj_cl==0) || (obj_cl->GetBaseClassOffset(TObject::Class()) != 0)) return;

      obj = (TObject*) obj_cl->New();
      if (obj==0) {
         printf("TGo4HttpAccess fail to create object of class %s\n", GetObjectClassName());
         return;
      }

      TBufferFile buf(TBuffer::kRead, res.size(), res.data(), kFALSE);
      buf.MapObject(obj, obj_cl);

      obj->Streamer(buf);

      // workaround - when ratemeter received, check running state
      if (obj->IsA() == TGo4Ratemeter::Class()) {
         fProxy->fbAnalysisRunning = ((TGo4Ratemeter*) obj)->IsRunning();
      }

      if ((fKind==6) && (obj!=0))
         fProxy->SetAnalysisReady(kTRUE);

   }

   DoObjectAssignement(fReceiver, fRecvPath.Data(), obj, kTRUE);
}

// =========================================================================

class TGo4HttpLevelIter : public TGo4LevelIter {
   protected:
      TXMLEngine      *fXML;
      XMLNodePointer_t fParent;
      XMLNodePointer_t fChild;

   public:
      TGo4HttpLevelIter(TXMLEngine* xml, XMLNodePointer_t item) :
         TGo4LevelIter(),
         fXML(xml),
         fParent(item),
         fChild()
         {
         }

      virtual ~TGo4HttpLevelIter() {}

      virtual Bool_t next()
      {
         if (fParent == 0) return kFALSE;

         while (true) {

            if (fChild == 0) {
               fChild = fXML->GetChild(fParent);
            } else {
               fChild = fXML->GetNext(fChild);
            }

            if (fChild==0) return kFALSE;

            if (fXML->HasAttr(fChild,"_hidden")) continue;

            break;
         }

         return fChild!=0;
      }

      virtual Bool_t isfolder() {
         return fXML->GetChild(fChild)!=0;
      }

      virtual Int_t getflag(const char* flagname)
      {
         if (strcmp(flagname,"IsRemote")==0) return 1;

         if (strcmp(flagname,"IsDeleteProtect")==0)
            return fXML->HasAttr(fChild, "_can_delete") ? 0 : 1;

         if (strcmp(flagname,"IsResetProtect")==0)
            return fXML->HasAttr(fChild, "_no_reset") ? 1 : 0;

         return -1;
      }

      virtual TGo4LevelIter* subiterator()
      {
         if (!isfolder()) return 0;
         return new TGo4HttpLevelIter(fXML,fChild);
      }

      virtual TGo4Slot* getslot() { return 0; }

      virtual const char* name()
      {
         const char* real = fXML->GetAttr(fChild,"_realname");
         return real!=0 ? real : fXML->GetAttr(fChild,"_name");
      }

      virtual const char* info() { return fXML->GetAttr(fChild,"_title"); }
      virtual Int_t sizeinfo() { return 0; }

      virtual Int_t GetKind() {
         if (isfolder()) return TGo4Access::kndFolder;

         if (fXML->HasAttr(fChild,"_go4event")) return TGo4Access::kndEventElement;

         if (fXML->HasAttr(fChild,"_more")) return TGo4Access::kndMoreFolder;

         const char* _kind = fXML->GetAttr(fChild,"_kind");
         if ((_kind!=0) && strcmp(_kind,"Command") == 0) return TGo4Access::kndRootCommand;

         const char* drawfunc = fXML->GetAttr(fChild,"_drawfunc");
         if ((drawfunc!=0) && !strcmp(drawfunc, "GO4.drawParameter")) return TGo4Access::kndGo4Param;

         const char* classname = GetClassName();

         if (classname==0) return TGo4Access::kndNone;

         if (strcmp(classname,"TLeafElement")==0) return TGo4Access::kndTreeLeaf;

         return TGo4Access::kndObject;
      }

      virtual const char* GetClassName()
      {
         const char* _kind = fXML->GetAttr(fChild,"_kind");
         const char* res = GetHttpRootClassName(_kind);
         if (res!=0) return res;
         if (_kind && !strcmp(_kind,"rate") && fXML->HasAttr(fChild,"_history")) return "TGraph";
         return isfolder() ? "TFolder" : 0;
      }
};



// =====================================================================

TGo4HttpProxy::TGo4HttpProxy() :
   TGo4ServerProxy(),
   fXML(0),
   fxHierarchy(0),
   fComm(this),
   fRateCnt(0),
   fStatusCnt(0),
   fDebugCnt(0),
   fAnalysisLaunched(0),
   fbAnalysisRunning(kFALSE),
   fUserName(),
   fPassword(),
   fConnected(kTRUE),
   fRegularReq(0)
{
   fXML = new TXMLEngine;
   fUserName = "anonymous";
   // SetAccount("observer","go4view");
   // SetAccount("controller","go4ctrl");
}

TGo4HttpProxy::~TGo4HttpProxy()
{
   fXML->FreeDoc(fxHierarchy);
   fxHierarchy = 0;

   delete fXML; fXML = 0;
}


void TGo4HttpProxy::SetAccount(const char* username, const char* passwd)
{
   fUserName = username ? username : "";
   fPassword = passwd ? passwd : "";
}

void TGo4HttpProxy::Initialize(TGo4Slot* slot)
{
   TGo4ServerProxy::Initialize(slot);

   if (!IsGo4Analysis()) return;

   TGo4Slot* subslot = new TGo4Slot(fxParentSlot, "Settings", "Analysis configuration");
   subslot->SetProxy(new TGo4ObjectProxy());

   subslot = new TGo4Slot(fxParentSlot, "Ratemeter", "Analysis ratemeter");
   subslot->SetProxy(new TGo4ObjectProxy());

   subslot = new TGo4Slot(fxParentSlot, "Loginfo", "Latest status messages");
   subslot->SetProxy(new TGo4ObjectProxy());

   subslot = new TGo4Slot(fxParentSlot, "Debugoutput", "Debug output of go4 analysis");
   subslot->SetProxy(new TGo4ObjectProxy());

   QTimer::singleShot(2000, &fComm, SLOT(updateRatemeter()));
}


XMLNodePointer_t TGo4HttpProxy::FindItem(const char* name, XMLNodePointer_t curr) const
{
   if (curr==0) curr = fXML->GetChild(fXML->DocGetRootElement(fxHierarchy));

   if ((curr==0) || (name==0) || (*name==0)) return curr;

   const char* slash = strchr(name,'\/');
   bool doagain = false;

   do {
      int len = (slash==0) ? strlen(name) : (slash - name);

      XMLNodePointer_t chld = fXML->GetChild(curr);
      while (chld!=0) {
         const char* _name = fXML->GetAttr(chld,"_realname");
         if (_name==0) _name = fXML->GetAttr(chld,"_name");

         if ((_name!=0) && (strncmp(_name, name, len)==0))
            return FindItem(slash ? slash+1 : 0, chld);

         chld = fXML->GetNext(chld);
      }

      // we try to process situation when item name contains slashes
      doagain = slash!=0;
      if (slash) slash = strchr(slash+1,'\/');

   } while (doagain);

   return 0;
}

TString TGo4HttpProxy::MakeUrlPath(XMLNodePointer_t item)
{
   if (item==0) return TString("");

   XMLNodePointer_t root = fXML->GetChild(fXML->DocGetRootElement(fxHierarchy));

   TString res;

   while (item != root) {
      const char* _name = fXML->GetAttr(item,"_name");
      if (_name==0) return TString("");
      if (res.Length()>0)
         res = TString(_name) + "/" + res;
      else
         res = _name;

      item = fXML->GetParent(item);
      if (item==0) return TString("");
   }

   return res;
}


void TGo4HttpProxy::GetHReply(QByteArray& res)
{
   if (res.size()==0) return;
   XMLDocPointer_t doc = fXML->ParseString(res.data());

   if (doc!=0) {
      fXML->FreeDoc(fxHierarchy);
      fxHierarchy = doc;
   }

   if (fxParentSlot!=0)
     fxParentSlot->ForwardEvent(fxParentSlot, TGo4Slot::evObjAssigned);
}

const char* TGo4HttpProxy::GetContainedObjectInfo()
{
   TGo4ServerProxy::GetContainedObjectInfo(); // evaluate roles
   fInfoStr +="(";
   fInfoStr +=GetUserName();
   fInfoStr +="@";
   fInfoStr +=GetServerName();
   fInfoStr +=")";
   const char* analname = fXML->GetAttr(FindItem(""), "_analysis_name");
   if (analname!=0) {
      fInfoStr += " name:";
      fInfoStr += analname;
   }

   return fInfoStr.Data();
}


Bool_t TGo4HttpProxy::Connect(const char* nodename)
{
   fNodeName = nodename;

   if ((fNodeName.Index("http://")!=0)  && (fNodeName.Index("https://")!=0))
      fNodeName = TString("http://") + fNodeName;

   return UpdateHierarchy(kTRUE);
}

Bool_t TGo4HttpProxy::NamesListReceived()
{
   return (fxHierarchy!=0) && (fComm.fHReply==0);
}


Bool_t TGo4HttpProxy::UpdateHierarchy(Bool_t sync)
{
   if (fComm.fHReply!=0) return kTRUE;

   TString req = fNodeName + "/h.xml?compact";

   fComm.StartRequest(req.Data());

   if (!sync) return kTRUE;

   QTime t;
   t.start();

   // wait several seconds
   while (t.elapsed() < 5000) {
      if (fComm.fHReply==0) break;
      QApplication::processEvents();
   }

   return fxHierarchy != 0;
}

Bool_t TGo4HttpProxy::HasSublevels() const
{
   return fxHierarchy != 0;
}

TGo4Access* TGo4HttpProxy::ProvideAccess(const char* name)
{
   XMLNodePointer_t item = FindItem(name);
   if (item==0) return 0;

   const char* _kind = fXML->GetAttr(item,"_kind");

   Int_t kind = 1;

   if (!strcmp(_kind,"rate") && fXML->HasAttr(item,"_history")) kind = 3; else
   if (fXML->HasAttr(item,"_dabc_hist")) kind = 2; else
   if (fXML->HasAttr(item,"_more")) kind = 0;

   return new TGo4HttpAccess(this, item, kind);
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

Bool_t TGo4HttpProxy::ServerHasRestrict()
{
   // return kTRUE when server has Restrict methods
   // It is indication of new functionality like commands with arguments or support of POST requests

   return fXML->HasAttr(FindItem(""),"_has_restrict");
}

Bool_t TGo4HttpProxy::ServerHasMulti()
{
   // return kTRUE when server has ProduceMulti method
   // One could use it to submit many requests at once

   return fXML->HasAttr(FindItem(""),"_has_produce_multi");
}


Bool_t TGo4HttpProxy::IsGo4Analysis() const
{
   XMLNodePointer_t item = FindItem("");
   if (item==0) return kFALSE;

   const char* _kind = fXML->GetAttr(item,"_kind");
   const char* _title = fXML->GetAttr(item,"_title");

   if ((_kind==0) || (_title==0)) return kFALSE;

   return !strcmp(_kind,"ROOT.Session") && !strcmp(_title,"GO4 analysis");
}

Bool_t TGo4HttpProxy::CheckUserName(const char* expects, Bool_t dflt)
{
   XMLNodePointer_t item = FindItem("");
   if (item==0) return dflt;

   const char* username = fXML->GetAttr(item,"_username");
   if (username==0) return dflt;

   return strcmp(username, expects)==0;
}


Bool_t TGo4HttpProxy::RefreshNamesList()
{
   return UpdateHierarchy(kFALSE);
}

Bool_t TGo4HttpProxy::DelayedRefreshNamesList(Int_t delay_sec)
{
   QTimer::singleShot(delay_sec*1000, &fComm, SLOT(updateHierarchy()));

   return kTRUE;
}

Bool_t TGo4HttpProxy::CanSubmitObjects()
{
   if (!IsGo4Analysis() || IsViewer()) return kFALSE;

   return ServerHasRestrict();
}


void TGo4HttpProxy::RequestAnalysisSettings()
{
   if (SubmitRequest("Control/Analysis", 6, SettingsSlot()))
   SetAnalysisSettingsReady(kTRUE);  // workaround - mark as we finished with settings
}

void TGo4HttpProxy::SubmitAnalysisSettings()
{
   TGo4AnalysisStatus* status = 0;
   if (SettingsSlot()!=0)
      status = dynamic_cast<TGo4AnalysisStatus*>(SettingsSlot()->GetAssignedObject());

   if (status)
      PostObject("Control/Analysis/exe.bin?method=ApplyStatus&status", status, 2);
}

void TGo4HttpProxy::CloseAnalysisSettings()
{
   SetAnalysisSettingsReady(kFALSE);

   SubmitCommand("Control/CmdClose");
}

void TGo4HttpProxy::ClearAllAnalysisObjects()
{
   // when command submitted without arguments, histograms and conditions folder will be cleared
   SubmitCommand("Control/CmdClear");
}

void TGo4HttpProxy::ClearAnalysisObject(const char* fullpath)
{
   TString foldername, objectname;
   TGo4Slot::ProduceFolderAndName(fullpath, foldername, objectname);

   objectname = TString("\"") + objectname + TString("\"");

   SubmitCommand("Control/CmdClearObject", -1, objectname.Data());
}

void TGo4HttpProxy::RemoveObjectFromAnalysis(const char* fullpath)
{
   TString foldername, objectname;
   TGo4Slot::ProduceFolderAndName(fullpath, foldername, objectname);

   objectname = TString("\"") + objectname + TString("\"");

   SubmitCommand("Control/CmdDeleteObject", -1, objectname.Data());
}

void TGo4HttpProxy::ExecuteLine(const char* line)
{
   // use method of TGo4AnalysisWebStatus - this works with all THttpServer versions
   if (FindItem("Control/Analysis"))
      SubmitURL(TString::Format("Control/Analysis/exe.json?method=ExecuteLine&cmd=\"%s\"", line));
}

void TGo4HttpProxy::StartAnalysis()
{
   SubmitCommand("Control/CmdStart");
   fbAnalysisRunning = kTRUE;
}

void TGo4HttpProxy::StopAnalysis()
{
   SubmitCommand("Control/CmdStop");
   fbAnalysisRunning = kFALSE;
}

Bool_t TGo4HttpProxy::RequestObjectStatus(const char* objectname, TGo4Slot* tgtslot)
{
   return SubmitRequest(objectname, 4, tgtslot) != 0;
}

Bool_t TGo4HttpProxy::SubmitURL(const char* path, Int_t waitres)
{
   TString url = fNodeName;
   url.Append("/");
   url.Append(path);

   if (gDebug>1) printf("Submit URL %s\n", url.Data());

   QNetworkReply *netReply = fComm.qnam.get(QNetworkRequest(QUrl(url.Data())));

   QSslConfiguration cfg = netReply->sslConfiguration();
   cfg.setProtocol(QSsl::AnyProtocol/*QSsl::TlsV1SslV3*/);
   netReply->setSslConfiguration(cfg);

   if (waitres<=0) {
      netReply->connect(netReply, SIGNAL(finished()), netReply, SLOT(deleteLater()));
      return kTRUE;
   }

   QEventLoop loop;
   QTime myTimer;
   myTimer.start();

   while (!netReply->isFinished()) {
      loop.processEvents(QEventLoop::AllEvents,100);
      if (myTimer.elapsed() > waitres*1000) break;
   }

   netReply->deleteLater();

   return netReply->isFinished();
}

Int_t  TGo4HttpProxy::NumCommandArgs(const char* name)
{
   XMLNodePointer_t item = FindItem(name);
   if (item==0) return -1;

   const char* _numargs = fXML->GetAttr(item,"_numargs");
   if (_numargs==0) return 0;

   return TString(_numargs).Atoi();
}

Bool_t TGo4HttpProxy::SubmitCommand(const char* name, Int_t waitres, const char* arg1, const char* arg2, const char* arg3)
{
   TString url(name);
   url.Append("/cmd.json");
   if ((arg1!=0) && (*arg1!=0)) {
      url.Append("?arg1=");
      url.Append(arg1);
      if ((arg2!=0) && (*arg2!=0)) {
         url.Append("&arg2=");
         url.Append(arg2);
         if ((arg3!=0) && (*arg3!=0)) {
            url.Append("&arg3=");
            url.Append(arg3);
         }
      }
   }

   return SubmitURL(url.Data(), waitres);
}

Bool_t TGo4HttpProxy::PostObject(const char* prefix, TObject* obj, Int_t waitres, Bool_t destroy_after)
{
   if (!ServerHasRestrict()) return kFALSE;

   TBufferFile *sbuf = new TBufferFile(TBuffer::kWrite, 100000);
   sbuf->MapObject(obj);
   obj->Streamer(*sbuf);

   QByteArray postData;
   postData.append(sbuf->Buffer(), sbuf->Length());

   delete sbuf;

   TString url = fNodeName;
   url.Append("/");
   url.Append(prefix);
   url.Append("=_post_object_&");
   if (destroy_after) url.Append("_destroy_post_&");
   url.Append("_post_class_=");
   url.Append(obj->ClassName());

   // printf("URL %s datalen %d\n", url.Data(), postData.length());

   QNetworkRequest req(QUrl(url.Data()));

   QNetworkReply *netReply = fComm.qnam.post(req, postData);

   QSslConfiguration cfg = netReply->sslConfiguration();
   cfg.setProtocol(QSsl::AnyProtocol/*QSsl::TlsV1SslV3*/);
   netReply->setSslConfiguration(cfg);

   if (waitres<=0) {
      netReply->connect(netReply, SIGNAL(finished()), netReply, SLOT(deleteLater()));
      return kTRUE;
   }

   QEventLoop loop;
   QTime myTimer;
   myTimer.start();

   while (!netReply->isFinished()) {
      loop.processEvents(QEventLoop::AllEvents,100);
      if (myTimer.elapsed() > waitres*1000) break;
   }

   netReply->deleteLater();

   return netReply->isFinished();
}


Bool_t TGo4HttpProxy::UpdateAnalysisObject(const char* objectname, TObject* obj)
{
   TString url;

   Bool_t destr = kTRUE;

   if (*objectname == 0) {
      url.Append("Control/go4_sniffer/exe.bin?method=AddAnalysisObject&obj");
      destr = kFALSE; // object will be owned by analysis
   } else {
      url.Append(objectname);
      url.Append("/exe.bin?method=");

      if (obj->InheritsFrom(TGo4Condition::Class())) {
         url.Append("UpdateFrom&counts=kFALSE&cond");
      } else {
         url.Append("SetStatus&status");
      }
   }

   return PostObject(url.Data(), obj, 2, destr);
}

void TGo4HttpProxy::ResetDebugOutputRequests()
{
   TGo4Slot* subslot = DebugOutputSlot();
   if (subslot) subslot->AssignObject(0, kFALSE);
}

void TGo4HttpProxy::ProcessRegularMultiRequest(Bool_t finished)
{
   if (finished) {
      if (fRegularReq == 0) return;

      QByteArray res = fRegularReq->readAll();
      fRegularReq->deleteLater();
      fRegularReq = 0;

      if (res.size() <= 0) {
         fConnected = false;

         // check if proxy is in shutdown phase
         if (CheckShutdown()) return;

         RatemeterSlot()->AssignObject(new TNamed("disconnected","title"),kTRUE);

         return;
      }

      if (!fConnected) {
         UpdateHierarchy(kFALSE);
         fConnected = true;
      }

      if (gDebug>2) printf("TGo4HttpProxy:: get reply on multi.bin request with %d bytes\n", res.size());

      int pos = 0;

      for (int n=0;n<3;n++) {
         unsigned char* ptr = (unsigned char*) res.data() + pos;
         unsigned len =  ((unsigned) ptr[0]) |
                        (((unsigned) ptr[1]) << 8) |
                        (((unsigned) ptr[2]) << 16) |
                        (((unsigned) ptr[3]) << 24);

         ptr += 4;
         pos += 4 + len;
         if (len > res.size()) {
            printf("Error decoding multi.bin buffer\n");
            return;
         }

         if (gDebug>2) printf("Decoding portion of %d bytes\n", len);

         TClass* obj_cl = n==0 ? TGo4Ratemeter::Class() : TList::Class();
         TObject* obj = (TObject*) obj_cl->New();

         TBufferFile buf(TBuffer::kRead, len, ptr, kFALSE);
         buf.MapObject(obj, obj_cl);

         obj->Streamer(buf);

         switch(n) {
            case 0:
               fbAnalysisRunning = ((TGo4Ratemeter*) obj)->IsRunning();
               RatemeterSlot()->AssignObject(obj,kTRUE);
               break;
            case 1:
               LoginfoSlot()->AssignObject(obj,kTRUE);
               break;
            case 2:
               DebugOutputSlot()->AssignObject(obj,kTRUE);
               break;
         }
      }

      if (pos != res.size()) printf("Decoding fails %d != %d bytes\n", pos, res.size());

      return;
   }


   if (fRegularReq!=0) return;

   TString req;

   req.Append("Ratemeter/root.bin\n");

   req.Append("Msg/exe.bin?method=Select&max=10000");
   TGo4Slot* subslot = LoginfoSlot();
   TList* curr = subslot ? dynamic_cast<TList*> (subslot->GetAssignedObject()) : 0;
   if (curr && curr->First()) {
      req.Append("&id=");
      req.Append(curr->First()->GetName());

   }
   req.Append("\n");

   req.Append("Log/exe.bin?method=Select&max=10000");
   subslot = DebugOutputSlot();
   curr = subslot ? dynamic_cast<TList*> (subslot->GetAssignedObject()) : 0;
   if (curr && curr->First()) {
      req.Append("&id=");
      req.Append(curr->First()->GetName());

   }
   req.Append("\n");

   QByteArray postData;
   postData.append(req.Data(), req.Length());

   if (gDebug>2) printf("Sending multi.bin request\n%s", req.Data());

   TString url = fNodeName;
   url.Append("/Status/multi.bin.gz?number=3");

   fRegularReq = fComm.qnam.post(QNetworkRequest(QUrl(url.Data())), postData);

   QSslConfiguration cfg = fRegularReq->sslConfiguration();
   cfg.setProtocol(QSsl::AnyProtocol/*QSsl::TlsV1SslV3*/);
   fRegularReq->setSslConfiguration(cfg);

   fRegularReq->connect(fRegularReq, SIGNAL(finished()), &fComm, SLOT(regularRequestFinished()));
}

Bool_t TGo4HttpProxy::CheckShutdown(Bool_t force)
{
   if (force || (fShutdownCnt>0)) {
      if (fxParentSlot) fxParentSlot->Delete();
      fxParentSlot = 0;
      return kTRUE;
   }
   return kFALSE;
}


void TGo4HttpProxy::ProcessUpdateTimer()
{
   if ((fShutdownCnt>0) && (--fShutdownCnt==0)) {
      CheckShutdown(kTRUE);
      return;
   }

   if (ServerHasMulti() || (fRegularReq!=0)) {
      ProcessRegularMultiRequest();
      return;
   }

   TGo4Slot* subslot = RatemeterSlot();
   if (subslot != 0) {
      // no new update since last call
      if ((subslot->GetAssignedObject()==0) || (fRateCnt != subslot->GetAssignCnt())) {
         fRateCnt = subslot->GetAssignCnt();
         SubmitRequest("Status/Ratemeter", 1, subslot);
      }
   }

   subslot = LoginfoSlot();
   if ((subslot!=0) && IsConnected()) {
      TList* curr = dynamic_cast<TList*> (subslot->GetAssignedObject());
      if ((curr==0) || (fStatusCnt != subslot->GetAssignCnt())) {
         TString arg;
         if (curr && curr->First()) {
            arg = "id=";
            arg += curr->First()->GetName();
         }
         fStatusCnt = subslot->GetAssignCnt();
         SubmitRequest("Status/Msg", 7, subslot, arg);
      }
   }

   subslot = DebugOutputSlot();
   if ((subslot!=0) && IsConnected()) {
      TList* curr = dynamic_cast<TList*> (subslot->GetAssignedObject());
      if ((curr==0) || (fDebugCnt != subslot->GetAssignCnt())) {
         TString arg;
         if (curr && curr->First()) {
            arg = "id=";
            arg += curr->First()->GetName();
         }
         fDebugCnt = subslot->GetAssignCnt();
         SubmitRequest("Status/Log", 7, subslot, arg);
      }
   }

}

void TGo4HttpProxy::RemoteTreeDraw(const char* treename,
                                   const char* varexp,
                                   const char* cutcond,
                                   const char* hname)
{
   TString tfoldername, tobjectname;
   TGo4Slot::ProduceFolderAndName(treename, tfoldername, tobjectname);

   TString hfoldername, hobjectname;
   TGo4Slot::ProduceFolderAndName(hname, hfoldername, hobjectname);

   TString path;
   path.Form("Control/go4_sniffer/exe.json?method=RemoteTreeDraw&histoname=\"%s\"&treename=\"%s\"&varexpr=\"%s\"&cutexpr=\"%s\"",
         hobjectname.Data(), tobjectname.Data(), varexp, cutcond);

   SubmitURL(path, 2);
}

TGo4HttpAccess* TGo4HttpProxy::SubmitRequest(const char* itemname, Int_t kind, TGo4Slot* tgtslot, const char* extra_arg)
{
   if ((itemname==0) || (tgtslot==0)) return 0;

   XMLNodePointer_t item = FindItem(itemname);
   if (item==0) return 0;

   if (kind==4) {
      // when status for histogram is requested, redirect request to the sniffer
      const char* _objkind = fXML->GetAttr(item, "_kind");
      if ((_objkind!=0) && ((strstr(_objkind, "ROOT.TH1")==_objkind) ||
            (strstr(_objkind, "ROOT.TH2")==_objkind) || (strstr(_objkind, "ROOT.TH3")==_objkind))) kind = 8;
   }

   TGo4HttpAccess* access = new TGo4HttpAccess(this, item, kind, extra_arg);
   access->AssignObjectToSlot(tgtslot); // request event itself

   return access;
}


void TGo4HttpProxy::RequestEventStatus(const char* evname, Bool_t astree, TGo4Slot* tgtslot)
{
   if (tgtslot==0) {
      // this is remote printing of event

      TString url = evname;
      url.Append("/exe.bin?method=");
      url.Append(astree ? "ShowSampleTree" : "PrintEvent");

      SubmitURL(url);
      return;
   }

   SubmitRequest(evname, astree ? 5 : 1, tgtslot);
}

void TGo4HttpProxy::RemotePrintEvent(const char* evname,
                                     Int_t evnumber,
                                     Int_t subid,
                                     Bool_t ishex,
                                     Bool_t islong)
{
   TString url;
   url.Form("Events/%s/exe.bin?method=SetPrintEvent&num=%d&sid=%d&longw=%d&hexw=%d&dataw=%d",
              evname, evnumber, subid, islong ? 1 : 0, ishex ? 1 : 0, ishex ? 0 : 1);
   SubmitURL(url);
}

void TGo4HttpProxy::ChageObjectProtection(const char* fullpath, const char* flags)
{
   unsigned reset_bits(0), set_bits(0);

   TString opt = flags;
   if(opt.Contains("+D")) reset_bits |= TGo4Status::kGo4CanDelete;
   if(opt.Contains("-D")) set_bits |= TGo4Status::kGo4CanDelete;
   if(opt.Contains("+C")) set_bits |= TGo4Status::kGo4NoReset;
   if(opt.Contains("-C")) reset_bits |= TGo4Status::kGo4NoReset;

   TString url(fullpath);
   url.Append("/exe.bin?method=");

   if (reset_bits!=0) SubmitURL(url + TString::Format("ResetBit&f=%u",reset_bits));
   if (set_bits!=0) SubmitURL(url + TString::Format("SetBit&f=%u&prototype=UInt_t",set_bits));
}

void TGo4HttpProxy::PrintDynListEntry(const char* fullpath)
{
   SubmitURL(TString(fullpath)+"/exe.bin?method=Print");
}

void TGo4HttpProxy::LoadConfigFile(const char* fname)
{
   SubmitURL(TString::Format("Control/Analysis/exe.bin?method=LoadStatus&fname=%s", fname));
}

void TGo4HttpProxy::SaveConfigFile(const char* fname)
{
   SubmitURL(TString::Format("Control/Analysis/exe.bin?method=SaveStatus&fname=%s", fname));
}

void TGo4HttpProxy::WriteAutoSave(const char* fname,
                                  Int_t complevel,
                                  Bool_t overwrite)
{
   SubmitURL(TString::Format("Control/Analysis/exe.bin?method=WriteAutoSave&fname=%s&overwrite=%s&complevel=%d", fname, overwrite ? "kTRUE" : "kFALSE", complevel));
}

void TGo4HttpProxy::DisconnectAnalysis(Int_t waittime, Bool_t servershutdown)
{
   if (servershutdown && IsGo4Analysis() && IsAdministrator()) {
      SubmitCommand("Control/CmdExit");
      fShutdownCnt = waittime;
   } else {
      CheckShutdown(kTRUE);
   }
}
