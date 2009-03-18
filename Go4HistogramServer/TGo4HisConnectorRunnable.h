#ifndef TGO4HISCONNECTORRUNNABLE_H
#define TGO4HISCONNECTORRUNNABLE_H

#include "TGo4Runnable.h"

class TGo4HistogramServer;

/** Runnable that waits for client connection request and
  * starts new HisCliRunnable. */
class TGo4HisConnectorRunnable : public TGo4Runnable {
   public:

      virtual ~TGo4HisConnectorRunnable();

      TGo4HisConnectorRunnable();

      TGo4HisConnectorRunnable(const char* name, TGo4HistogramServer* hserv);

      virtual Int_t Run(void*);

   private:

      /** @supplierCardinality 1
       * @clientCardinality 1*/
      TGo4HistogramServer * fxHistogramServer;  //!
};

#endif //TGO4HISCONNECTORRUNNABLE_H
