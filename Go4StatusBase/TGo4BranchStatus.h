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

#ifndef TGO4BRANCHSTATUS_H
#define TGO4BRANCHSTATUS_H

#include "TGo4ObjectStatus.h"

class TBranch;

class TGo4BranchStatus : public TGo4ObjectStatus {
   public:
      TGo4BranchStatus();

      /**
       * Create status information from given TBranch object.
       * For reconstruction of TTree structure for display in remote browser.
       */
      TGo4BranchStatus(TBranch* branch);

      virtual ~TGo4BranchStatus();


      virtual Int_t PrintStatus(Text_t* buffer=0, Int_t buflen=0);


   ClassDef(TGo4BranchStatus,1)
};

#endif //TGO4BRANCHSTATUS_H
