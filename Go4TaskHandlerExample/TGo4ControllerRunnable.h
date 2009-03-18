#ifndef TGO4CONTROLLERRUNNABLE_H
#define TGO4CONTROLLERRUNNABLE_H

#include "TGo4Runnable.h"

class TGo4ExampleController;
class TGo4ExampleServer;

/**
 * runnable performing the main action of example client task:
 * getting commands out of command queue, sending objects into data queue
 * @stereotype thread
 */
class TGo4ControllerRunnable : public TGo4Runnable {

  public:

    TGo4ControllerRunnable(const char* name, TGo4ExampleServer* serv);

    virtual ~TGo4ControllerRunnable();

    virtual Int_t Run(void*);

   /** frequency in ms to acquire new command from controller instance */
   static const UInt_t fguCOMMANDINTERVAL;//!

  private:

    /** @supplierCardinality 1 */
   TGo4ExampleController * fxController; //!
   TGo4ExampleServer* fxServer; //!

   TGo4ControllerRunnable();
};

#endif //TGO4CONTROLLERRUNNABLE_H
