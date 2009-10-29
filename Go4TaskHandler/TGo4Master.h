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

#ifndef TGO4MASTER_H
#define TGO4MASTER_H

#include "TGo4TaskOwner.h"

#include "TString.h"
#include "TGo4Command.h"
#include "Go4EmergencyCommands.h"

class TGo4TaskHandlerCommandList;

class TGo4Master :  public TGo4TaskOwner {
  public:
    TGo4Master() ;

    TGo4Master(const char* name, Bool_t isserver=kTRUE, const char* serverhost="localhost", UInt_t negotport=5000) ;

    virtual ~TGo4Master() ;

    /** send command specified by name to the current slave task */
    Bool_t SubmitCommand(const char* name);

    /** send emergency quit command to the current slave task */
    Bool_t SubmitEmergencyCommand(Go4EmergencyCommand_t val);

    /** send given command to the current slave task. Command object is deleted after
      * this call internally! */
    Bool_t SubmitCommand(TGo4Command* com);

    /** factory method for command list; overridden by implementation */
    virtual TGo4TaskHandlerCommandList*  CreateCommandList();

    virtual Bool_t DisconnectSlave(const char* name=0,
                                   Bool_t waitforslave=kTRUE );

private:

    /**
     * Name of the node where the slave is running. Used to kill it later.
     */
    TString fxSlaveNode;

    /**
     * Name of the  shell where the slave is running. Used to kill it later.
     */
    TString fxSlaveShell;

    /**
     * Name of the executable (command) that is running in the slave process.
     * Used to kill it later.
     */
    TString fxSlaveExec;
};
#endif //TGO4MASTER_H
