// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

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

