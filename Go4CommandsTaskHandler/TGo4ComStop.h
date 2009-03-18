#ifndef TGO4COMSTOP_H
#define TGO4COMSTOP_H

#include "TGo4TaskHandlerCommand.h"

/**
 * Command calling the virtual method Stop() of Client Task class;
 *    this method is overridden by user client, thus any stop action of the application
 *    may be invoked by this (e.g. stop of analysis main event loop)
 */

class TGo4ComStop : public TGo4TaskHandlerCommand {

  public:

    TGo4ComStop();

    virtual ~TGo4ComStop();

    Int_t ExeCom();

  ClassDef(TGo4ComStop,1)
};

#endif //TGO4COMSTOP_H
