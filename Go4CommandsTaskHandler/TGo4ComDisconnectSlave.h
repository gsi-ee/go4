#ifndef TGO4COMDISCONNECTSLAVE_H
#define TGO4COMDISCONNECTSLAVE_H

#include "TGo4TaskHandlerCommand.h"
#include "TString.h"

/**
 * master command: current slave
 *  is removed (disconnected) from server
 * @since 17-03-2005
 * @author Joern Adamczewski
 */

class TGo4ComDisconnectSlave : public TGo4TaskHandlerCommand {
  public:

    TGo4ComDisconnectSlave();

    virtual ~TGo4ComDisconnectSlave();

    /** Specify the name of the client to be removed */
    void SetSlave(const char* name) { fxSlaveName = name; }

    void SetWaitForSlave(Bool_t wait=kTRUE) { fbWaitForSlave=wait; }

    Int_t ExeCom();

  private:

    /** name of the client to be removed */
    TString fxSlaveName;

    Bool_t fbWaitForSlave;

   ClassDef(TGo4ComDisconnectSlave,1);
};

#endif //TGO4COMDISCONNECTSLAVE_H
