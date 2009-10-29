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

#ifndef TGO4MEMBERSTATUS_H
#define TGO4MEMBERSTATUS_H

#include "TGo4ObjectStatus.h"

class TGo4MemberStatus : public TGo4ObjectStatus {
   public:
      TGo4MemberStatus();

      /**
       * Create status information about event datamemeber.
       * For reconstruction of eventstructure for display in remote browser.
       */
      TGo4MemberStatus(const char* name, const char* datatypename);

      virtual ~TGo4MemberStatus();

      virtual Int_t PrintStatus(Text_t* buffer=0, Int_t buflen=0);


   ClassDef(TGo4MemberStatus,1)
};

#endif //TGO4MEMBERSTATUS_H
