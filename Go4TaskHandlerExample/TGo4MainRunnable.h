#ifndef TGO4MAINRUNNABLE_H
#define TGO4MAINRUNNABLE_H

#include "TGo4Runnable.h"

#include "TGo4ExampleApplication.h"
#include "TGo4ExampleClient.h"

/**
 * runnable performing the main action of example client task:
 * getting commands out of command queue, sending objects into data queue
 * @stereotype thread
 */

class TGo4MainRunnable : public TGo4Runnable {
  public:

    TGo4MainRunnable(const char* name, TGo4ExampleClient* cli);

    virtual ~TGo4MainRunnable();

    virtual Int_t Run(void*);

  private:
    /** @supplierCardinality 1
     * @directed
     * @clientCardinality 0..1*/
    TGo4ExampleApplication * fxApplication;//!

    TGo4MainRunnable();
};

#endif //TGO4MAINRUNNABLE_H
