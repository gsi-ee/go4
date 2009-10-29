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

#ifndef TGO4STATUSRUNNABLE_H
#define TGO4STATUSRUNNABLE_H

#include "TGo4TaskHandlerRunnable.h"

class TGo4ThreadManager;
class TGo4TaskHandler;

/**
 * Runnable responsible for status object  exchange between transport (socket) and buffer (queue)
 *   In receiver mode, status objects are received from transport and passed to the queue
 *  in non receiver mode, runnables waits for the queue and sends all status objects from the queue
 *  over the transport
 * @stereotype thread
 */

class TGo4StatusRunnable : public TGo4TaskHandlerRunnable {
   public:

      TGo4StatusRunnable(const char* name, TGo4ThreadManager* man, TGo4TaskHandler* hand, Bool_t receivermode=kTRUE);

      virtual ~TGo4StatusRunnable();

      virtual Int_t Run(void* ptr);

   private:
      TGo4StatusRunnable();
};

#endif //TGO4STATUSRUNNABLE_H
