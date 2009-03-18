#ifndef TGO4TASKHANDLEREXCEPTION_H
#define TGO4TASKHANDLEREXCEPTION_H

#include "TGo4ControlException.h"

class TGo4TaskHandler;
class TGo4TaskHandlerRunnable;
class TGo4ThreadManager;

class TGo4TaskHandlerException : public TGo4ControlException {
   public:
     TGo4TaskHandlerException (TGo4TaskHandler* taskhandler);

     TGo4TaskHandlerException(TGo4TaskHandlerRunnable* run);

     virtual ~TGo4TaskHandlerException();

     virtual Int_t Handle () = 0;

  protected:

      TGo4TaskHandler * fxTaskHandler; //!

      TGo4ThreadManager * fxThreadManager; //!

      TGo4TaskHandlerRunnable * fxCaller; //!

  private:
      TGo4TaskHandlerException();
};

#endif // #define TGO4TASKHANDLEREXCEPTION_H
