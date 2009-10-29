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

#ifndef TGO4COMMANDRUNNABLE_H
#define TGO4COMMANDRUNNABLE_H

#include "TGo4TaskHandlerRunnable.h"

class TGo4ThreadManager;
class TGo4TaskHandler;
class TGo4CommandInvoker;

/**
 * Runnable responsible for command exchange between transport (socket) and buffer (queue)
 *  In receiver mode, commands are received from transport, checked if sync or not, the
 *  command receiver of system (client) commands is set to the clienttask, sync commands are executed
 *  directly, async commands are passed to the queue
 * in not receiver mode, runnables waits for the queue and sends all commands from the queue
 * over the transport
 * @stereotype thread
 */

class TGo4CommandRunnable : public TGo4TaskHandlerRunnable {
   public:

      TGo4CommandRunnable(const char* name, TGo4ThreadManager* man, TGo4TaskHandler* hand, Bool_t receivermode=kTRUE);

      virtual ~TGo4CommandRunnable();

      virtual Int_t Run(void* ptr);

   private:
      TGo4CommandRunnable();

      /** for error in socket: redirect cout/cerr to dummy buffer to
       * avoid trouble if client is running in dying QProcess */
      void RedirectIO();

      /**
       * client mode: link to external (static) invoker singleton, that is
       * used to execute the async commands before the command queue
       * server mode: not used
       */
      TGo4CommandInvoker* fxInvoker; //!
};

#endif //TGO4COMMANDRUNNABLE_H
