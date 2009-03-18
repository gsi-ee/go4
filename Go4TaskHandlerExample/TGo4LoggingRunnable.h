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
