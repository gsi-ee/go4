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

#ifndef TGO4HTTPPROXY_H
#define TGO4HTTPPROXY_H

#include "TGo4ServerProxy.h"

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
      QNetworkAccessManager qnam;          //! central manager of network requests
      QNetworkReply *fHReply{nullptr};     //! used only to receive hierarchy
      TGo4HttpProxy *fProxy{nullptr};

   public slots:
      void httpFinished();
      void httpHReqError(QNetworkReply::NetworkError);
      void authenticationRequiredSlot(QNetworkReply*, QAuthenticator*);

      void updateRatemeter();
      void updateHierarchy();

      void regularRequestFinished();

   public:

      QHttpProxy(TGo4HttpProxy* p);
      virtual ~QHttpProxy();

      void StartRequest(const char* url);
};

// -----------------------------------------------------------------------------------

class TGo4HttpAccess : public QObject, public TGo4Access {

   Q_OBJECT

   protected:
      TGo4HttpProxy   *fProxy{nullptr};
      XMLNodePointer_t fNode{nullptr};
      TString          fUrlPath;
      // Request kind. Can be:
      //   0 - h.xml request,
      //   1 - root.bin request,
      //   2 - dabc get.xml request,
      //   3 - dabc get.xml history requests
      //   4 - parameter status request
      //   5 - sample tree
      //   6 - analysis status request
      Int_t            fKind{0};
      TString          fNameAttr;
      TString          fKindAttr;
      TString          fExtraArg;
      TGo4ObjectManager* fReceiver{nullptr};
      TString          fRecvPath;
      QNetworkReply   *fReply{nullptr};

   public slots:
      void httpFinished();
      void httpError(QNetworkReply::NetworkError);

   public:

      TGo4HttpAccess(TGo4HttpProxy* proxy, XMLNodePointer_t node, Int_t kind = 1, const char* extra_arg = nullptr);

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
class TGo4HttpProxy : public TGo4ServerProxy {

   friend class QHttpProxy;
   friend class TGo4HttpAccess;

   protected:
      TXMLEngine     *fXML{nullptr};
      XMLDocPointer_t fxHierarchy{nullptr};    //!  pointer on dabc::Hierarchy class
      QHttpProxy      fComm;                   //!
      Int_t           fRateCnt{0};             //! counter for ratemeter updates
      Int_t           fStatusCnt{0};           //! counter for status messages updates
      Int_t           fDebugCnt{0};            //! counter for debug output updates
      Bool_t          fbAnalysisRunning{kFALSE}; //!
      TString         fUserName;               //! user name and password -
      TString         fPassword;               //!
      Bool_t          fConnected{kFALSE};      //! true if connected
      QNetworkReply  *fRegularReq{nullptr};    //! multiple request for rate, log and messages
      Int_t           fShutdownCnt{0};         //! counter during shutdown

      void GetHReply(QByteArray& res);

      XMLNodePointer_t FindItem(const char* name, XMLNodePointer_t curr = nullptr) const;

      TString MakeUrlPath(XMLNodePointer_t item);

      void ProcessUpdateTimer();

      void ProcessRegularMultiRequest(Bool_t finished = kFALSE);

      Bool_t ServerHasRestrict();

      Bool_t ServerHasMulti();

      Bool_t SubmitURL(const char* path, Int_t waitres = -1);

      Bool_t PostObject(const char* prefix, TObject* obj, Int_t waitres = -1, Bool_t destroy_after = kTRUE);

      Bool_t CheckUserName(const char* expects, Bool_t dflt = kFALSE);

      Bool_t CheckShutdown(Bool_t force = kFALSE);

      TGo4HttpAccess* SubmitRequest(const char* itemname, Int_t kind, TGo4Slot* tgtslot, const char* extra_arg = nullptr);

   public:
      TGo4HttpProxy();
      virtual ~TGo4HttpProxy();

      void SetAccount(const char* username, const char* passwd);

      Bool_t Connect(const char* nodename);
      Bool_t UpdateHierarchy(Bool_t sync = kTRUE);

      void Initialize(TGo4Slot* slot) override;
      void Finalize(TGo4Slot* slot) override {}

      Bool_t HasSublevels() const override;
      TGo4Access* ProvideAccess(const char* name) override;
      TGo4LevelIter* MakeIter() override;

      Int_t GetObjectKind() override {  return TGo4Access::kndFolder; }
      const char* GetContainedClassName() override { return ClassName(); }
      TObject* GetAssignedObject() override { return this; }
      const char* GetContainedObjectInfo() override;
      Int_t GetObjectSizeInfo() override { return -1; }

      void WriteData(TGo4Slot* slot, TDirectory* dir, Bool_t onlyobjs) override;
      void ReadData(TGo4Slot* slot, TDirectory* dir) override;

      void Update(TGo4Slot* slot, Bool_t strong) override;

      TString FindCommand(const char* name) override;
      Int_t  NumCommandArgs(const char* name) override;
      Bool_t SubmitCommand(const char* name, Int_t waitres = -1, const char* arg1 = nullptr, const char* arg2 = nullptr, const char* arg3 = nullptr) override;

      virtual const char* GetUserName() const { return fUserName.Data(); }

      Bool_t IsAnalysisServer() const override { return kTRUE; }
      Bool_t IsGo4Analysis() const override;
      Bool_t IsConnected() override { return fConnected; }
      Bool_t IsViewer() override { return CheckUserName("observer", kFALSE); }
      Bool_t IsController() override { return CheckUserName("controller", kFALSE); }
      Bool_t IsAdministrator() override { return CheckUserName("admin", kTRUE); }

      Bool_t CanSubmitObjects() override;
      void RequestAnalysisSettings() override;
      void SubmitAnalysisSettings() override;
      void CloseAnalysisSettings() override;

      Bool_t NamesListReceived() override;
      Bool_t RefreshNamesList() override;
      Bool_t DelayedRefreshNamesList(Int_t delay_sec) override;

      Bool_t IsAnalysisRunning() const override { return fbAnalysisRunning; }
      void StartAnalysis() override;
      void StopAnalysis() override;

      Bool_t RequestObjectStatus(const char* objectname, TGo4Slot* tgtslot) override;

      Bool_t UpdateAnalysisObject(const char* objectname, TObject* obj) override;

      void ClearAllAnalysisObjects() override;
      void ClearAnalysisObject(const char* fullpath) override;

      void RemoteTreeDraw(const char* treename,
                          const char* varexp,
                          const char* cutcond,
                          const char* hname) override;

      void RequestEventStatus(const char* evname, Bool_t astree, TGo4Slot* tgtslot) override;

      void RemotePrintEvent(const char* evname,
                            Int_t evnumber,
                            Int_t subid,
                            Bool_t ishex,
                            Bool_t islong) override;

      void ChageObjectProtection(const char* fullpath, const char* flags) override;

      void RemoveObjectFromAnalysis(const char* fullpath) override;

      void ExecuteLine(const char* line) override;

      void PrintDynListEntry(const char* fullpath) override;

      void LoadConfigFile(const char* fname) override;
      void SaveConfigFile(const char* fname) override;

      void WriteAutoSave(const char* fname,
                         Int_t complevel,
                         Bool_t overwrite) override;

      void ResetDebugOutputRequests() override;

      void DisconnectAnalysis(Int_t waittime = 30, Bool_t servershutdown = kFALSE) override;


};

#endif
