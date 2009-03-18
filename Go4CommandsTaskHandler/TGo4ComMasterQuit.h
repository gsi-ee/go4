#ifndef TGO4COMMASTERQUIT_H
#define TGO4COMMASTERQUIT_H

#include "TGo4TaskHandlerCommand.h"

/**
 * Master Command to quit (terminate) the master application. If master is in server mode, all slave clients
 * are shut down before master quits. If master is connected as client to a slave server, this command will not terminate the slave.
 * @author Joern Adamczewski
 * @since 17-03-2005
 */

class TGo4ComMasterQuit : public TGo4TaskHandlerCommand {
  public:

    TGo4ComMasterQuit();

    virtual ~TGo4ComMasterQuit();

    Int_t ExeCom();


  ClassDef(TGo4ComMasterQuit,1);
};

#endif //TGO4COMMASTERQUIT_H
