#ifndef TGO4OBJCONNECTORRUNNABLE_H
#define TGO4OBJCONNECTORRUNNABLE_H

#include "TGo4Runnable.h"

class TGo4HistogramServer;

/**
 * Runnable that serves an object request for object client
 * @stereotype thread
 */
class TGo4ObjConnectorRunnable : public TGo4Runnable {
   public:

      virtual ~TGo4ObjConnectorRunnable();

      TGo4ObjConnectorRunnable();

      TGo4ObjConnectorRunnable(const char* name, TGo4HistogramServer* hserv);

     virtual Int_t Run(void*);

   private:

      /** @supplierCardinality 1
       * @clientCardinality 1*/
      TGo4HistogramServer * fxHistogramServer;//!
};

#endif //TGO4OBJCONNECTORRUNNABLE_H
