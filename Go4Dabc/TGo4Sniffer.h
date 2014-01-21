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

class TGo4Sniffer : public TRootSniffer {
   public:
      TGo4Sniffer(const char* name, Int_t comp);

      virtual ~TGo4Sniffer() {}

      virtual void ScanRoot(TRootSnifferScanRec& rec);
};


#endif
