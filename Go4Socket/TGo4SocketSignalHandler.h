#ifndef TGO4SOCKETSIGNALHANDLER_H
#define TGO4SOCKETSIGNALHANDLER_H

#include "Rtypes.h"

#ifdef WIN32
#define SIGWINCH 28
#else
#include <signal.h>
#endif


class TGo4SocketSignalHandler {
   public:

      TGo4SocketSignalHandler(Int_t signum, Bool_t enabled=kTRUE);

      virtual ~TGo4SocketSignalHandler();

      static void SetSignalAction(Int_t signum, Bool_t enabled=kTRUE);

      static void Handle(int);

      static Int_t GetLastSignal();

      static void SetLastSignal(Int_t v = 0);

   private:
      TGo4SocketSignalHandler();

      static Int_t fgiLastSignal; //! contains last signal handler result
};

#endif //TGO4SOCKETSIGNALHANDLER_H
