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
