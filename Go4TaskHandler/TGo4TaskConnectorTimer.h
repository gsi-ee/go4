#ifndef TGO4TASKCONNECTORTIMER_H
#define TGO4TASKCONNECTORTIMER_H

#include "TTimer.h"

class TGo4ServerTask;

/**
 * timer aggregated to the servertask which is responsible to
 * open and handle the negotiation requests of the clients
 */

class TGo4TaskConnectorTimer : public TTimer {

  public:

      TGo4TaskConnectorTimer (TGo4ServerTask* server, Long_t msperiod = 3000);

      virtual ~TGo4TaskConnectorTimer();

      virtual Bool_t Notify ();

  protected:

      /** The servertask to which this timer belongs */
      TGo4ServerTask* fxServer; //!

  private:
      TGo4TaskConnectorTimer();

};

#endif //TGO4TASKCONNECTORTIMER_H
