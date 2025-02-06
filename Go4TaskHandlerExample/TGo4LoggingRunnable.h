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

#ifndef TGO4LOGGINGRUNNABLE_H
#define TGO4LOGGINGRUNNABLE_H

#include "TGo4Runnable.h"

class TGo4ExampleController;
class TGo4ExampleServer;

/**
 * runnable performing control watcher functions of example client task:
 *  status of application is send to status queue
  */

class TGo4LoggingRunnable : public TGo4Runnable {
  public:
    TGo4LoggingRunnable(const char *name, TGo4ExampleServer* serv);

    virtual ~TGo4LoggingRunnable();

    Int_t Run(void *) override;

  private:
    /** @supplierCardinality 1
     * @directed*/
    TGo4ExampleController *fxController{nullptr}; //!

    TGo4ExampleServer *fxServer{nullptr}; //!

    TGo4LoggingRunnable();
};

#endif //TGO4LOGGINGRUNNABLE_H
