#ifndef TGO4COMREMOVECLIENT_H
#define TGO4COMREMOVECLIENT_H

#include "TGo4TaskHandlerCommand.h"
#include "TString.h"

/**
 * server command: current client is removed (disconnected) from server
 */

class TGo4ComRemoveClient : public TGo4TaskHandlerCommand {
  public:

    TGo4ComRemoveClient();

    virtual ~TGo4ComRemoveClient();

    /** Specify the name of the client to be removed */
    void SetClient(const char* name);

    void SetWaitForClient(Bool_t wait=kTRUE);

    Int_t ExeCom();

  private:

    /** name of the client to be removed */
    TString fxClientName;

    Bool_t fbWaitForClient;

  ClassDef(TGo4ComRemoveClient,1)
};

#endif //TGO4COMREMOVECLIENT_H
