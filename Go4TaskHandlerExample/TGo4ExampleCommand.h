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

#ifndef TGO4EXAMPLECOMMAND_H
#define TGO4EXAMPLECOMMAND_H

#include "TGo4Command.h"

class TGo4ExampleCommand : public TGo4Command {
  public:
    TGo4ExampleCommand();
    TGo4ExampleCommand(const char* name, const char* desc);
    virtual ~TGo4ExampleCommand();
    TGo4ExampleCommand(const TGo4ExampleCommand &right);

   ClassDef(TGo4ExampleCommand,1)
};

#endif //TGO4EXAMPLECOMMAND_H
