#ifndef TGO4CONNECTORRUNNABLE_H
#define TGO4CONNECTORRUNNABLE_H

#include "TGo4Runnable.h"

class TGo4TaskManager;
class TGo4ServerTask;

/**
 * Runnable to handle connection request from a new client which connects
 * to a listening Transport channel (socket) on a fixed port number (e.g. 5000).
 * When client asks the connector runnable for  a new task handler connection
 *  (in method Connect of client task handler in client mode), this runnable tells
 * him the first port number and machine name of the server, creates a new
 * task handler in server mode, adds it to the task manager  and invokes
 * the server task handler's  Connect. Then the three transport channels with
 * the correct port numbers should be established...
 * @stereotype thread
 */

class TGo4ConnectorRunnable : public TGo4Runnable {
  public:

      TGo4ConnectorRunnable(const char* name, TGo4ServerTask* man);

      virtual ~TGo4ConnectorRunnable();

      virtual Int_t Run(void* ptr);

   private:
      TGo4ConnectorRunnable();

      /**
       * @supplierCardinality 1
       * @directed
       */
      TGo4TaskManager * fxTaskManager; //!

};

#endif //TGO4CONNECTORRUNNABLE_H
