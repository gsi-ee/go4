#ifndef TGO4OBJCONNECTORTIMER_H
#define TGO4OBJCONNECTORTIMER_H

#include "TTimer.h"

class TGo4HistogramServer;

/**
 * timer aggregated to the histogram server which is responsible to
 * establish connections of the go4 object clients.
 */

class TGo4ObjConnectorTimer : public TTimer {

  public:

      TGo4ObjConnectorTimer (TGo4HistogramServer* server, Long_t msperiod = 3000);

      virtual ~TGo4ObjConnectorTimer();

      virtual Bool_t Notify ();

  protected:

      /** the object server to which this timer belongs */
      TGo4HistogramServer* fxServer; //!

  private:
      TGo4ObjConnectorTimer();
};

#endif //TGO4TASKCONNECTORTIMER_H
