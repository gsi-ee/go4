// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4TaskHandlerCommandList.h"

#include "TGo4ComQuit.h"
#include "TGo4ComStart.h"
#include "TGo4ComStop.h"
#include "TGo4ComExecLine.h"
#include "TGo4ComServerQuit.h"
#include "TGo4ComRemoveClient.h"
#include "TGo4ComMasterQuit.h"
#include "TGo4ComDisconnectSlave.h"

TGo4TaskHandlerCommandList::TGo4TaskHandlerCommandList(const char *name) :
   TGo4CommandProtoList(name)
{
    // all valid TaskHandler commands should be added to prototype list...
    AddCommand(new TGo4ComQuit);
    AddCommand(new TGo4ComMasterQuit);
    AddCommand(new TGo4ComStart);
    AddCommand(new TGo4ComStop);
    AddCommand(new TGo4ComDisconnectSlave);
    AddCommand(new TGo4ComServerQuit);
    AddCommand(new TGo4ComRemoveClient);
    AddCommand(new TGo4ComExecLine);
}

TGo4TaskHandlerCommandList::~TGo4TaskHandlerCommandList()
{
}
