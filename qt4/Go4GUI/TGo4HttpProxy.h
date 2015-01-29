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

#include "TGo4Proxy.h"
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
      QNetworkReply *fReply;
      TGo4HttpProxy *fProxy;

   public slots:
      void httpFinished();
      void httpError(QNetworkReply::NetworkError);
      void httpSslErrors ( const QList<QSslError> & errors);

   public:

      QHttpProxy(TGo4HttpProxy* p) : QObject(), qnam(), fReply(0), fProxy(p) {}
      virtual ~ QHttpProxy() {}

      void StartRequest(const char* url);

};

// -----------------------------------------------------------------------------------

class TGo4HttpAccess : public QObject, public TGo4Access {

   Q_OBJECT

   protected:
      TGo4HttpProxy   *fProxy;
      XMLNodePointer_t fNode;
      TString          fPath;
      Int_t            fKind; // 0 - h.xml request, 1 - root.bin request, 2 - dabc get.xml request
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
      TGo4Slot*       fxParentSlot;
      QHttpProxy      fComm;

      void GetReply(QByteArray& res);

      XMLNodePointer_t FindItem(XMLNodePointer_t curr, const char* name);

   public:
      TGo4HttpProxy();
      virtual ~TGo4HttpProxy();

      Bool_t Connect(const char* nodename);
      Bool_t UpdateHierarchy(Bool_t sync = kTRUE);

      virtual void Initialize(TGo4Slot* slot) { fxParentSlot = slot; }
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
      virtual Bool_t RefreshNamesList();
};

#endif
