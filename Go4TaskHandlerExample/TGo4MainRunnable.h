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

#ifndef TGO4MAINRUNNABLE_H
#define TGO4MAINRUNNABLE_H

#include "TGo4Runnable.h"

#include "TGo4ExampleApplication.h"
#include "TGo4ExampleClient.h"

/**
 * runnable performing the main action of example client task:
 * getting commands out of command queue, sending objects into data queue
 *
 */

class TGo4MainRunnable : public TGo4Runnable {
  public:

    TGo4MainRunnable(const char *name, TGo4ExampleClient* cli);

    virtual ~TGo4MainRunnable();

    Int_t Run(void *) override;

  private:
    /** @supplierCardinality 1
     * @directed
     * @clientCardinality 0..1*/
    TGo4ExampleApplication *fxApplication{nullptr};//!

    TGo4MainRunnable();
};

#endif //TGO4MAINRUNNABLE_H
