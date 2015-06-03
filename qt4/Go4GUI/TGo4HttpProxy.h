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

#ifndef TGO4HTTPPROXY_H
#define TGO4HTTPPROXY_H

#include "TGo4ServerProxy.h"
#include "TString.h"
#include "TXMLEngine.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

class TGo4HttpProxy;
class TGo4HttpAccess;
class TGo4ObjectManager;

/** Here Qt-specific functionality of HttpProxy */
class QHttpProxy : public QObject {
   Q_OBJECT

   friend class TGo4HttpProxy;
   friend class TGo4HttpAccess;

   protected:
      QNetworkAccessManager qnam;  //! central manager of network requests
      QNetworkReply *fHReply;      //! used only to receive hierarchy
      TGo4HttpProxy *fProxy;

   public slots:
      void httpFinished();
      void httpError(QNetworkReply::NetworkError);
      void authenticationRequiredSlot(QNetworkReply*, QAuthenticator*);

      void updateRatemeter();
      void updateHierarchy();

   public:

      QHttpProxy(TGo4HttpProxy* p);
      virtual ~QHttpProxy();

      void StartRequest(const char* url);
};

// -----------------------------------------------------------------------------------

class TGo4HttpAccess : public QObject, public TGo4Access {

   Q_OBJECT

   protected:
      TGo4HttpProxy   *fProxy;
      XMLNodePointer_t fNode;
      TString          fPath;
      // Request kind. Can be:
      //   0 - h.xml request,
      //   1 - root.bin request,
      //   2 - dabc get.xml request,
      //   3 - dabc get.xml history requests
      //   4 - parameter status request
      Int_t            fKind;
      TString          fNameAttr;
      TString          fKindAttr;
      TGo4ObjectManager* fReceiver;
      TString          fRecvPath;
      QNetworkReply   *fReply;

   public slots:
      void httpFinished();

   public:

      TGo4HttpAccess(TGo4HttpProxy* proxy, XMLNodePointer_t node, const char* path, Int_t kind = 1);

      virtual ~TGo4HttpAccess() { }

      virtual Bool_t IsRemote() const { return kTRUE; }

      virtual Bool_t CanGetObject() const { return kFALSE; }

      virtual Bool_t GetObject(TObject* &obj, Bool_t &owner) const { return kFALSE; }

      virtual TClass* GetObjectClass() const;

      virtual const char* GetObjectName() const;

      virtual const char* GetObjectClassName() const;

      virtual Int_t AssignObjectTo(TGo4ObjectManager* rcv, const char* path);

};

// -----------------------------------------------------------------------------------

/** Here Go4/ROOT-specific functionality of HttpProxy */
class TGo4HttpProxy : public TGo4ServerProxy  {

   friend class QHttpProxy;
   friend class TGo4HttpAccess;

   protected:
      TString         fNodeName;
      TXMLEngine     *fXML;
      XMLDocPointer_t fxHierarchy;    //!  pointer on dabc::Hierarchy class
      QHttpProxy      fComm;
      Int_t           fRateCnt;       //! counter for ratemeter updates
      Bool_t          fbAnalysisRunning;
      TString         fUserName;     //! user name and password -
      TString         fPassword;

      void GetReply(QByteArray& res);

      XMLNodePointer_t FindItem(const char* name, XMLNodePointer_t curr = 0) const;

      void ProcessUpdateTimer();

      Bool_t SubmitCommand(const char* name, Int_t waitres = -1, const char* par1 = 0);

      Bool_t PostObject(const char* prefix, TObject* obj, Int_t waitres = -1, Bool_t destroy_after = kTRUE);

      Bool_t CheckUserName(const char* expects, Bool_t dflt = kFALSE);

   public:
      TGo4HttpProxy();
      virtual ~TGo4HttpProxy();

      void SetAccount(const char* username, const char* passwd);

      Bool_t Connect(const char* nodename);
      Bool_t UpdateHierarchy(Bool_t sync = kTRUE);

      virtual void Initialize(TGo4Slot* slot);
      virtual void Finalize(TGo4Slot* slot) {}

      virtual Bool_t HasSublevels() const;
      virtual TGo4Access* ProvideAccess(const char* name);
      virtual TGo4LevelIter* MakeIter();

      virtual Int_t GetObjectKind() {  return TGo4Access::kndFolder; }
      virtual const char* GetContainedClassName() { return "TGo4ServerProxy"; }
      virtual const char* GetContainedObjectInfo() { return 0; }
      virtual Int_t GetObjectSizeInfo() { return -1; }

      virtual void WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs);
      virtual void ReadData(TGo4Slot* slot, TDirectory* dir);

      virtual void Update(TGo4Slot* slot, Bool_t strong);

      virtual const char* GetServerName() const { return fNodeName.Data(); }

      virtual Bool_t IsGo4Analysis() const;
      virtual Bool_t IsConnected();
      virtual Bool_t IsViewer()  { return CheckUserName("observer", kFALSE); }
      virtual Bool_t IsController()  { return CheckUserName("controller", kTRUE); }
      virtual Bool_t IsAdministrator()  { return CheckUserName("admin", kFALSE); }

      virtual void RequestAnalysisSettings();
      virtual void SubmitAnalysisSettings();
      virtual void CloseAnalysisSettings();

      virtual Bool_t RefreshNamesList();
      virtual Bool_t DelayedRefreshNamesList(Int_t delay_sec);

      virtual Bool_t IsAnalysisRunning() const { return fbAnalysisRunning; }
      virtual void StartAnalysis();
      virtual void StopAnalysis();

      virtual Bool_t RequestObjectStatus(const char* objectname, TGo4Slot* tgtslot);

      virtual Bool_t UpdateAnalysisObject(const char* objectname, TObject* obj);

      virtual void ClearAnalysisObject(const char* fullpath);

};

#endif
