#ifndef TGO4COMQUIT_H
#define TGO4COMQUIT_H

#include "TGo4TaskHandlerCommand.h"

/** Command to quit (terminate) the client,
  * after successful quit, the client may be removed from taskmanager on the
  * server side */

class TGo4ComQuit : public TGo4TaskHandlerCommand {
  public:

    TGo4ComQuit();

    virtual ~TGo4ComQuit();

    Int_t ExeCom();

  ClassDef(TGo4ComQuit,1)
};

#endif //TGO4COMQUIT_H
