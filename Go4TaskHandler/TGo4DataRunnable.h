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

#ifndef TGO4DATARUNNABLE_H
#define TGO4DATARUNNABLE_H

#include "TGo4TaskHandlerRunnable.h"

class TGo4ThreadManager;
class TGo4TaskHandler;

/**
 * Runnable responsible for data object  exchange between transport (socket) and buffer (queue)
 *   In receiver mode, data objects are received from transport and passed to the queue
 *  in non receiver mode, runnables waits for the queue and sends all data objects from the queue
 *  over the transport
 * @stereotype thread
 */

class TGo4DataRunnable : public TGo4TaskHandlerRunnable {

  public:

      TGo4DataRunnable(const char* name, TGo4ThreadManager* man, TGo4TaskHandler* hand, Bool_t receivermode=kTRUE);

      virtual ~TGo4DataRunnable();

      virtual Int_t Run(void* ptr);

   private:
      TGo4DataRunnable();

};

#endif //TGO4DATARUNNABLE_H
