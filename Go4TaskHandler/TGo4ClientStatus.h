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

#ifndef TGO4CLIENTSTATUS_H
#define TGO4CLIENTSTATUS_H

#include "TGo4TaskStatus.h"
#include "TString.h"


class TGo4ClientStatus : public TGo4TaskStatus {
   friend class TGo4ClientTask;

   public:

      TGo4ClientStatus();

      virtual ~TGo4ClientStatus();

      /**
       * basic method to printout status information
       * on stdout; to be overridden by specific
       * subclass
       */
      virtual Int_t PrintStatus(Text_t* buffer=0, Int_t buflen=0);

   protected:

      TGo4ClientStatus(const char* name);

      void SetNames(const char* serverhost);

   protected:

      /** hostname of server machine */
      TString fxServerHostName;

   ClassDef(TGo4ClientStatus,1)
};

#endif //TGO4CLIENTSTATUS_H
