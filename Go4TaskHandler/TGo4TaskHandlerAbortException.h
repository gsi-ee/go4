#ifndef TGO4TASKHANDLERABORTEXCEPTION_H
#define TGO4TASKHANDLERABORTEXCEPTION_H

#include "TGo4TaskHandlerException.h"

class TGo4TaskHandler;
class TGo4TaskHandlerRunnable;

class TGo4TaskHandlerAbortException : public TGo4TaskHandlerException {

  public:

   /**
    *   Exception to Disconnect client belonging to taskhandler from server task (servermode)
    *    or shut down the client task (clientmode)
    */
   TGo4TaskHandlerAbortException (TGo4TaskHandler* taskhandler);

   /**
    * ctor to call exception from within a taskhandler runnable;
    * will find out taskhandler from runnable
    */
   TGo4TaskHandlerAbortException(TGo4TaskHandlerRunnable * run);

   virtual ~TGo4TaskHandlerAbortException();

   Int_t Handle ();

private:

   TGo4TaskHandlerAbortException();
};

#endif //TGO4TASKHANDLERABORTEXCEPTION_H
