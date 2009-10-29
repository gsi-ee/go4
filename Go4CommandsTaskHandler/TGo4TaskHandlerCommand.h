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

#ifndef TGO4TASKHANDLERCOMMAND_H
#define TGO4TASKHANDLERCOMMAND_H

#include "TGo4Command.h"

class TGo4TaskHandlerCommand : public TGo4Command {
  public:

   TGo4TaskHandlerCommand();

   TGo4TaskHandlerCommand(const char* name, const char* description);

   virtual ~TGo4TaskHandlerCommand();

   TGo4TaskHandlerCommand(const TGo4TaskHandlerCommand &right);

   /** Optional action if command is not allowed (warning message e.g.) */
    virtual Int_t RefuseCom();

  ClassDef(TGo4TaskHandlerCommand,1)
};

#endif //TGO4TASKHANDLERCOMMAND_H
