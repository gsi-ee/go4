#ifndef TGO4COMACTION2_H
#define TGO4COMACTION2_H

#include "TGo4ExampleCommand.h"

/**
 * example command calling action routine 2 of example application
 */

class TGo4ComAction2 : public TGo4ExampleCommand {
  public:

    TGo4ComAction2();

    virtual ~TGo4ComAction2();

    Int_t ExeCom();

  ClassDef(TGo4ComAction2,1)
};

#endif //TGO4COMACTION2_H
