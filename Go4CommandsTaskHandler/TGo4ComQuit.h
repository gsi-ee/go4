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

#ifndef TGO4COMQUIT_H
#define TGO4COMQUIT_H

#include "TGo4TaskHandlerCommand.h"

/** Command to quit (terminate) the client,
  * after successful quit, the client may be removed from taskmanager on the
  * server side */

class TGo4ComQuit : public TGo4TaskHandlerCommand {
  public:

    TGo4ComQuit();

    virtual ~TGo4ComQuit();

    Int_t ExeCom();

  ClassDef(TGo4ComQuit,1)
};

#endif //TGO4COMQUIT_H
