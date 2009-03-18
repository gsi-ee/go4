#ifndef TGO4COMSERVERQUIT_H
#define TGO4COMSERVERQUIT_H

#include "TGo4TaskHandlerCommand.h"

/**
 * Server Command to quit (terminate) the server and the application
 * all clients are removed before
 */

class TGo4ComServerQuit : public TGo4TaskHandlerCommand {
  public:

    TGo4ComServerQuit();

    virtual ~TGo4ComServerQuit();

    Int_t ExeCom();


  ClassDef(TGo4ComServerQuit,1)
};

#endif //TGO4COMSERVERQUIT_H
