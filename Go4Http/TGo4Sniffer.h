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


#ifndef TGO4DABCSNIFFER_H
#define TGO4DABCSNIFFER_H

#include "TRootSniffer.h"

#include "TGo4AnalysisSniffer.h"

class TGo4AnalysisWebStatus;
class TGraph;
class TGo4Ratemeter;

class TGo4Sniffer : public TRootSniffer,
                    public TGo4AnalysisSniffer {
   protected:

      TGo4AnalysisWebStatus*  fAnalysisStatus;

      TGraph* fEventRate;

      TGo4Ratemeter* fRatemeter;

      virtual void ScanObjectProperties(TRootSnifferScanRec &rec, TObject *obj);

      /** Send message to gui */
      virtual void SendStatusMessage(Int_t level, Bool_t printout, const TString& text);

      static THttpServer* gHttpServer;

   public:

      static THttpServer* GetHttpServer() { return gHttpServer; }

      static Bool_t CreateEngine(const char* name);


      TGo4Sniffer(const char* name);
      virtual ~TGo4Sniffer();

      virtual void ScanRoot(TRootSnifferScanRec& rec);

      virtual void *FindInHierarchy(const char *path, TClass **cl = 0, TDataMember **member = 0, Int_t *chld = 0);

      Bool_t CmdStart();
      Bool_t CmdStop();
      Bool_t CmdClear();
      Bool_t CmdRestart();
      Bool_t CmdOpenFile(const char* fname);
      Bool_t CmdCloseFiles();
      Bool_t CmdClearObject(const char* objname);


      /** Method called by logger with every string, going to output */
      virtual void SetTitle(const char* title = "");

      /** Method from analysis sniffer */
      virtual void RatemeterUpdate(TGo4Ratemeter*);

      /** Method from analysis sniffer */
      virtual void StatusMessage(int level, const TString&);

      /** Method called in go4 analysis thread, used to executed server commands */
      virtual void ProcessSnifferEvents();


   ClassDef(TGo4Sniffer, 1);
};


#endif
