#ifndef TGO4COMACTION1_H
#define TGO4COMACTION1_H

#include "TGo4ExampleCommand.h"

/**
 * example command calling action routine 1 of example application
 */

class TGo4ComAction1 : public TGo4ExampleCommand {

  public:

    TGo4ComAction1();

    virtual ~TGo4ComAction1();

    Int_t ExeCom();

    ClassDef(TGo4ComAction1,1)
};

#endif //TGO4COMACTION1_H

