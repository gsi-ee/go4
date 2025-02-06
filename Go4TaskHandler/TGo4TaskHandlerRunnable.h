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

#ifndef TGO4TASKHANDLERRUNNABLE_H
#define TGO4TASKHANDLERRUNNABLE_H

#include "TGo4Runnable.h"

class TGo4Socket;
class TGo4ThreadManager;
class TGo4TaskHandler;
class TGo4ObjectQueue;
class TGo4BufferQueue;

/**
 * This is the abstract interface for all runnables associated
 * with the taskhandler class
 * @ingroup go4_taskhandler
 */

class TGo4TaskHandlerRunnable : public TGo4Runnable {

   public:

      TGo4TaskHandlerRunnable(const char *name, TGo4ThreadManager *man, TGo4TaskHandler *hand, Bool_t receivermode=kTRUE);

      virtual ~TGo4TaskHandlerRunnable();

      Int_t Run(void *ptr) override = 0;

      TGo4TaskHandler *GetTaskHandler();

   protected:

      /** Test if buffer contains a message to stop this
        * runnable. This mechanism is used to properly put
        * taskhandler threads into parking position when
        * connection is discarded. Value of buffer is
        * set to external variable result optionally. */
      Bool_t CheckStopBuffer(TBuffer *buf, Int_t *result = nullptr);

      /** Test if socket transport exists and is open.
        * If not open, sleep a while to avoid deadlooping of Run() */
      Bool_t CheckTransportOpen();

      /** link to external inter-tasl transport channel */
      TGo4Socket *fxTransport{nullptr};//!

      /** link to external task handler (for server mode)
        * @supplierCardinality 1
        * @clientCardinality 1 */
      TGo4TaskHandler *fxTaskHandler{nullptr}; //!

      /** flag indicating operation mode of runnable,
        * depending on client or server mode of task handler */
      Bool_t fbReceiverMode{kFALSE};

      /** Link to external buffer queue. For object sending mode. */
      TGo4BufferQueue *fxBufferQueue{nullptr}; //!

    private:
      TGo4TaskHandlerRunnable();
};

#endif //TGO4TASKHANDLERRUNNABLE_H
