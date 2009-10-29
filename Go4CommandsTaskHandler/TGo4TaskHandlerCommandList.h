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

#ifndef TGO4TASKHANDLERCOMMANDLIST_H
#define TGO4TASKHANDLERCOMMANDLIST_H

#include "TGo4CommandProtoList.h"
/**
 * Class containing all command prototypes of the
 * TaskHandler system commands (and the basic test commands);
 * may be derived by application or user command list class which
 * can add own commands into the list using AddCommand Method
 * of TGo4CommandProtoList Base Class
 */
class TGo4TaskHandlerCommandList : public TGo4CommandProtoList {
  public:

    TGo4TaskHandlerCommandList(const char* name);
    virtual ~TGo4TaskHandlerCommandList();

  private:
    TGo4TaskHandlerCommandList();

   ClassDef(TGo4TaskHandlerCommandList,1)
};

#endif //TGO4TASKHANDLERCOMMANDLIST_H
