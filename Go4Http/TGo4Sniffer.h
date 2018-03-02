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

#ifndef TGO4SNIFFER_H
#define TGO4SNIFFER_H

#ifdef SNIFFER_FULL
#include "TRootSnifferFull.h"
#define SniffBaseClass TRootSnifferFull
#else
#include "TRootSniffer.h"
#define SniffBaseClass TRootSniffer
#endif

#include "TGo4AnalysisSniffer.h"

#include "TGo4MsgList.h"

class TGo4AnalysisWebStatus;
class TGraph;
class TGo4Ratemeter;
class THttpServer;

class TGo4Sniffer : public SniffBaseClass,
                    public TGo4AnalysisSniffer {

   protected:

      TGo4AnalysisWebStatus*  fAnalysisStatus;

      TGraph* fEventRate;

      TGo4Ratemeter* fRatemeter;

      TGo4MsgList fDebugOutput;

      TGo4MsgList fStatusMessages;

      /** if true, python binding of go4 has already been done.*/
      Bool_t fbPythonBound;

      virtual void ScanObjectProperties(TRootSnifferScanRec &rec, TObject *obj);

      /** Send message to gui - need to be implemented */
      virtual void SendStatusMessage(Int_t level, Bool_t printout, const TString& text);

      /** Indicate that Restrict method implemented, also commands with arguments are exists */
      Bool_t HasRestrictMethod();

      /** Indicate that sniffer has MultiProcess method implemented, can be used from GUI */
      Bool_t HasProduceMultiMethod();

      /** Wrapper for new method in TRootSniffer */
      void RestrictGo4(const char* path, const char* options);

      /** Indicate if SetAutoLoad method exists */
      Bool_t HasAutoLoadMethod();

      /** Wrapper for new method in TRootSniffer */
      Bool_t SetAutoLoadGo4(const char* script);

      static THttpServer* gHttpServer;


   public:

      static THttpServer* GetHttpServer() { return gHttpServer; }

      static Bool_t CreateEngine(const char* name);


      TGo4Sniffer(const char* name);
      virtual ~TGo4Sniffer();

      virtual void ScanRoot(TRootSnifferScanRec& rec);

      virtual void *FindInHierarchy(const char *path, TClass **cl = 0, TDataMember **member = 0, Int_t *chld = 0);

      // ========= methods registered as command and available from web interface =========

      Bool_t CmdStart();
      Bool_t CmdStop();
      Bool_t CmdClear();
      Bool_t CmdRestart();
      Bool_t CmdClose();
      Bool_t CmdOpenFile(const char* fname);
      Bool_t CmdCloseFiles();
      Bool_t CmdExit();
      Bool_t CmdClearObject(const char* objname);
      Bool_t CmdDeleteObject(const char* objname);
      Bool_t CmdExecute(const char* exeline);

      // === methods used via http interface by Go4GUI ===============

      Bool_t AddAnalysisObject(TObject* obj);

      Bool_t RemoteTreeDraw(const char* histoname,
                            const char* treename,
                            const char* varexpr,
                            const char* cutexpr);

      TObject* CreateItemStatus(const char* itemname);

      /** Method called by logger with every string, going to output */
      virtual void SetTitle(const char* title = "");

      /** Method from analysis sniffer */
      virtual void RatemeterUpdate(TGo4Ratemeter*);

      /** Method from analysis sniffer */
      virtual void StatusMessage(int level, Bool_t printout, const TString&);

      /** Method called in go4 analysis thread, used to executed server commands */
      virtual void ProcessSnifferEvents();


   ClassDef(TGo4Sniffer, 1); // Sniffer of Go4 analysis objects for THttpServer
};


#endif
