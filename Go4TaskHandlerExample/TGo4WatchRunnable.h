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

#ifndef TGO4WATCHRUNNABLE_H
#define TGO4WATCHRUNNABLE_H

#include "TGo4Runnable.h"
#include "TGo4ExampleApplication.h"
#include "TGo4ExampleClient.h"

/**
 * runnable performing control watcher functions of example client task:
 *  status of application is send to status queue
 */

class TGo4WatchRunnable : public TGo4Runnable {
  public:

    TGo4WatchRunnable(const char *name, TGo4ExampleClient* cli);

    virtual ~TGo4WatchRunnable();

    Int_t Run(void *) override;

    /** frequency in ms for the watcher runnable to check the application */
    static const UInt_t fguWATCHINTERVAL;//!

  private:

    /** @supplierCardinality 1
     * @directed
     * @clientCardinality 0..1*/
    TGo4ExampleApplication *fxApplication{nullptr};//!

    TGo4WatchRunnable();
};

#endif //TGO4WATCHRUNNABLE_H
