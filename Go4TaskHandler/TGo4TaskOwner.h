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

#ifndef TGO4TASKOWNER_H
#define TGO4TASKOWNER_H

#include "TGo4CommandReceiver.h"
#include "RtypesCore.h"

class TMutex;
class TGo4Task;
class TGo4ServerTask;

/** Baseclass for all instances that can own a TGo4Task.
 * Bridge between client/server functionality and master/slave.
 * @ingroup go4_taskhandler
 * @since 17-03-2005
 * @author Joern Adamczewski  */

class TGo4TaskOwner : public TGo4CommandReceiver {
   friend class TGo4Task;
   friend class TGo4ServerTask;

   public:

      TGo4TaskOwner();

      TGo4TaskOwner(Bool_t isserver);

      virtual ~TGo4TaskOwner();

      Bool_t IsServer() const { return fbServerMode; }

      TGo4Task* GetTask() const { return fxTask; }

      /** Start the working threads of the task owner implementation;
        * this method is used after the current connection has changed, to continue reading/writing
        * the queues. To be overridden. */
      virtual Int_t StartWorkThreads();

      /** stop the working threads of the task owne implementation;
       * this method is used before the current connection is changed to prevent reading/writing
       * and waiting for the wrong queues. To be overridden. */
      virtual Int_t StopWorkThreads();

      /** Quit this instance and the owned task;
        * method to be called from
        * command object, may be overridden in user
        * implementation */
      virtual void Quit();

      const char *GetName() const;

      Bool_t IsConnected();

      Bool_t IsBeingQuit();

      /** Access to taskmanager mutex; to be locked outside main mutex */
      TMutex *GetTaskManagerMutex();


   protected:

      void SetTask(TGo4Task* task, Bool_t delold = kTRUE);

   private:

      /** @link aggregation
        * @supplierCardinality 1
        * @clientCardinality 1*/
      TGo4Task* fxTask{nullptr};

      /** Flag if master uses servertask or client task connection. */
    Bool_t fbServerMode{kFALSE};
};

#endif //TGO4TASKOWNER_H
