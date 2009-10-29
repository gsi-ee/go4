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

#ifndef TGO4LOGGINGRUNNABLE_H
#define TGO4LOGGINGRUNNABLE_H

#include "TGo4Runnable.h"

#include "TGo4ExampleController.h"
#include "TGo4ExampleServer.h"

/**
 * runnable performing control watcher functions of example client task:
 *  status of application is send to status queue
 * @stereotype thread
 */

class TGo4LoggingRunnable : public TGo4Runnable {
  public:
    TGo4LoggingRunnable(const char* name, TGo4ExampleServer* serv);

    virtual ~TGo4LoggingRunnable();

    virtual Int_t Run(void*);

  private:
    /** @supplierCardinality 1
     * @directed*/
    TGo4ExampleController * fxController; //!

    TGo4ExampleServer* fxServer; //!

    TGo4LoggingRunnable();
};

#endif //TGO4LOGGINGRUNNABLE_H
