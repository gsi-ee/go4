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

#ifndef TGO4TASKHANDLEREXCEPTION_H
#define TGO4TASKHANDLEREXCEPTION_H

#include "TGo4ControlException.h"

class TGo4TaskHandler;
class TGo4TaskHandlerRunnable;
class TGo4ThreadManager;

/** Task handler exception
 * @ingroup go4_taskhandler
 */
class TGo4TaskHandlerException : public TGo4ControlException {
   public:
     TGo4TaskHandlerException() = delete;
     TGo4TaskHandlerException (TGo4TaskHandler *taskhandler);

     TGo4TaskHandlerException(TGo4TaskHandlerRunnable* run);

     virtual ~TGo4TaskHandlerException();

     Int_t Handle() override = 0;

  protected:

      TGo4TaskHandler * fxTaskHandler{nullptr}; //!

      TGo4ThreadManager * fxThreadManager{nullptr}; //!

      TGo4TaskHandlerRunnable * fxCaller{nullptr}; //!
};

#endif // #define TGO4TASKHANDLEREXCEPTION_H
