#ifndef TGO4COMSTART_H
#define TGO4COMSTART_H

#include "TGo4TaskHandlerCommand.h"

/**
 * Command calling the virtual method Start() of Client Task class;
 *  this method is overridden by user client, thus any start action of the application
 *  may be invoked by this (e.g. start of analysis main event loop)
 */

class TGo4ComStart : public TGo4TaskHandlerCommand {

  public:

    TGo4ComStart();

    virtual ~TGo4ComStart();

    Int_t ExeCom();

  ClassDef(TGo4ComStart,1)
};

#endif //TGO4COMSTART_H
