// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4CONTROLLERRUNNABLE_H
#define TGO4CONTROLLERRUNNABLE_H

#include "TGo4Runnable.h"

class TGo4ExampleController;
class TGo4ExampleServer;

/**
 * runnable performing the main action of example client task:
 * getting commands out of command queue, sending objects into data queue
 * @ingroup go4_taskhandler
 */
class TGo4ControllerRunnable : public TGo4Runnable {

  public:

    TGo4ControllerRunnable(const char *name, TGo4ExampleServer* serv);

    virtual ~TGo4ControllerRunnable();

    Int_t Run(void *) override;

   /** frequency in ms to acquire new command from controller instance */
   static const UInt_t fguCOMMANDINTERVAL;//!

  private:

    /** @supplierCardinality 1 */
   TGo4ExampleController *fxController{nullptr}; //!
   TGo4ExampleServer *fxServer{nullptr}; //!

   TGo4ControllerRunnable();
};

#endif //TGO4CONTROLLERRUNNABLE_H
