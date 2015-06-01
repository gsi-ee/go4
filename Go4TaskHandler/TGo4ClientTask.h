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

#ifndef TGO4CLIENTTASK_H
#define TGO4CLIENTTASK_H

#include "TGo4Task.h"

#include "TGo4Command.h"
#include "TString.h"

class TMutex;

class TGo4Command;
class TGo4BufferQueue;
class TGo4TaskHandler;

/**
 * This class controls a client application running in a task which is controlled
 *  by another server task via transport channels which are held by a task handler object.
 * Apart from the transport threads, the application class which inherits from client task
 * may provide working threads to serve the data queues and do the actual work
 */
class TGo4ClientTask : public TGo4Task {

   public:

      TGo4ClientTask(const char* name,
                     const char* serverhost="localhost",
                     UInt_t negotiationport=0,
                     Bool_t blockingmode=kTRUE,
                     Bool_t standalone=kFALSE,
                     Bool_t autostart=kFALSE,
                     Bool_t autocreate=kTRUE,
                     Bool_t ismaster=kFALSE,
                     Bool_t autoconnect=kTRUE);

      virtual ~TGo4ClientTask();

      /**
       * override the ThreadManager Initialization used by AppControlTimer
       *  to let this thing initialize the client transport connection
       */
      virtual Int_t Initialization();

      /**
       * Quit the client; method to be called from
       * command object, may be overridden in user
       * implementation
       */
      virtual void Quit();

      /**
       * create a status object with information on the
       * current client state. this object may be sent to the
       * server or may be used to restore current status by
       * memento mechanism
       */
      virtual TGo4TaskStatus* CreateStatus();

      TGo4TaskHandler* GetTaskHandler();

      /**
       * Put command into the command queue for local execution in main thread
       */
      void AddLocalCommand(TGo4Command * com);

      TGo4BufferQueue* GetCommandQueue(const char* task=0);

      TGo4BufferQueue* GetStatusQueue(const char* task=0);

      TGo4BufferQueue* GetDataQueue(const char* task=0);

      /**
       * Connect this client to a waiting server task on node with
       * negotiation channel negport. Returns connection state,
       * true if connection is established, otherwise false.
       */
      Bool_t ConnectServer(const char* node, UInt_t negport,
                            Go4CommandMode_t role=kGo4ComModeRefused,
                            const char* passwd=0);

      /**
       * Disconnect the current server task, but do not terminate the client.
       * Return value indicates success state: true if disconnected, false if
       * connection is still existing. If isterminating=true, work threads
       * are not restarted again.
       */
      Bool_t DisconnectServer(Bool_t isterminating=kFALSE);

      const char* GetServerHostName(){return fxServerHostname.Data();}

      Bool_t IsConnected() const { return fbServerConnected; }

   protected:

      /**
       * method needed by method
       * CreateStatus of any status subclasses to
       * set the values of the clienttask specific
       * part of the status object.
       */
      virtual void UpdateStatus(TGo4TaskStatus* state);

   private:
      TGo4BufferQueue* fxStatusQ; //!
      TGo4BufferQueue* fxDataQ; //!
      TGo4BufferQueue* fxCommandQ; //!

      /** @link aggregationByValue
       * @directed
       * @clientCardinality 1
       * @supplierCardinality 1*/
      TGo4TaskHandler* fxTaskHandler; //!

      /** hostname of server machine */
      TString fxServerHostname;
      /**
       * !True if client task shall try to connect automatically
       * at Initialization. This is the previous behaviour with
       * standalone client. If false, connection will be done on ConnectServer method.
       */
      Bool_t fbAutoConnect;

      /**
       * true if connection to server is established, otherwise false
       */
      Bool_t fbServerConnected;
   ClassDef(TGo4ClientTask,1)
};

#endif //TGO4CLIENTTASK_H
