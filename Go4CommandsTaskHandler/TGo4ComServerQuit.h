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

#ifndef TGO4COMSERVERQUIT_H
#define TGO4COMSERVERQUIT_H

#include "TGo4TaskHandlerCommand.h"

/**
 * Server Command to quit (terminate) the server and the application
 * all clients are removed before
 */

class TGo4ComServerQuit : public TGo4TaskHandlerCommand {
  public:

    TGo4ComServerQuit();

    virtual ~TGo4ComServerQuit();

    Int_t ExeCom();


  ClassDef(TGo4ComServerQuit,1)
};

#endif //TGO4COMSERVERQUIT_H
