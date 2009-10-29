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

#ifndef TGO4COMMANDPROTOLIST_H
#define TGO4COMMANDPROTOLIST_H

/**
 * @author M. Hemberger
 */

#include "TNamed.h"

class TMutex;
class TObjArray;

class TGo4Command;
class TGo4RemoteCommand;

class TGo4CommandProtoList : public TNamed {
   public:

    TGo4CommandProtoList(const char* name);

    virtual ~TGo4CommandProtoList();

    /** Display all commands in list by name and description */
    virtual void ShowCommands();

    /** Creates a Clone of the specified command by name */
    TGo4Command* MakeCommand(const char* name);

    /** Creates command by specifications in remote command  */
    TGo4Command* MakeCommand(TGo4RemoteCommand* remcon);

    /** removes command from list */
    void RemoveCommand(const char* name);

    /** Adds a Command to the list */
    void AddCommand(TGo4Command* com);

    /** Adds second command list to this */
    TGo4CommandProtoList& operator+=(const TGo4CommandProtoList& two);

  private:
    TGo4CommandProtoList();

    /** @link aggregation
        *   @supplierCardinality 1 */
    TObjArray * fxCommandList;  //!

    /** @link aggregation
     *   @supplierCardinality 1 */
    TMutex * fxListMutex; //!

   ClassDef(TGo4CommandProtoList,1)
};

#endif //TGO4COMMANDPROTOLIST_H
