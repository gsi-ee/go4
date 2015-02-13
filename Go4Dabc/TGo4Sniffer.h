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

class TGo4AnalysisStatus;

class TGo4Sniffer : public TRootSniffer {
   protected:

      TGo4AnalysisStatus*  fAnalysisStatus;

      virtual void ScanObjectProperties(TRootSnifferScanRec &rec, TObject* &obj);

   public:
      TGo4Sniffer(const char* name);

      virtual ~TGo4Sniffer() {}

      virtual void ScanRoot(TRootSnifferScanRec& rec);

      virtual void *FindInHierarchy(const char *path, TClass **cl = 0, TDataMember **member = 0, Int_t *chld = 0);

};


#endif
