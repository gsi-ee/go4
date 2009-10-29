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

#ifndef TGO4TASKSTATUS_H
#define TGO4TASKSTATUS_H

#include "TGo4Status.h"

class TGo4TaskHandlerStatus;

class TGo4TaskStatus : public TGo4Status {
   friend class TGo4Task;
   friend class TGo4Slave;

   public:

      TGo4TaskStatus();

      virtual ~TGo4TaskStatus();

      Bool_t IsInitDone() const { return fbInitDone; }

      Bool_t IsTerminating() const { return fbTerminating; }

      Bool_t IsAutoStart() const { return fbAutoStart; }

      Bool_t IsAutoCreate() const { return fbAutoCreate; }

      Bool_t IsAppBlocking() const { return fbAppBlocking; }

      TGo4TaskHandlerStatus* GetTaskHandlerStatus() const { return fxTaskHandlerStatus; }

      /** method to printout status information on stdout; */
      virtual Int_t PrintStatus(Text_t* buffer=0, Int_t buflen=0);

   protected:

      void SetFlags(Bool_t blocking, Bool_t autocreate, Bool_t autostart, Bool_t terminating, Bool_t initdone);

      void SetTaskHandlerStatus(TGo4TaskHandlerStatus* thstate);

      Bool_t fbAppBlocking;
      Bool_t fbAutoCreate;
      Bool_t fbAutoStart;
      Bool_t fbTerminating;

      /** @link aggregationByValue
       *   @supplierCardinality 1 */
      TGo4TaskHandlerStatus * fxTaskHandlerStatus;
      Bool_t fbInitDone;

      TGo4TaskStatus(const char* name);

   ClassDef(TGo4TaskStatus,1)
};

#endif //TGO4TASKSTATUS_H
