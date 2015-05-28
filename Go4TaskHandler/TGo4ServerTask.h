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

#ifndef TGO4SERVERTASK_H
#define TGO4SERVERTASK_H

#include "TGo4Task.h"

class TGo4TaskManager;
class TGo4TaskHandler;
class TGo4BufferQueue;
class TGo4ObjectQueue;
class TGo4Command;
class TGo4Socket;
class TGo4Status;
class TGo4TaskConnectorTimer;

class TGo4ServerTask : public TGo4Task {
   public:

      TGo4ServerTask(const char* name,
            UInt_t negotiationport=0,
            Bool_t blockingmode=kFALSE,
            Bool_t standalone=kFALSE,
            Bool_t autostart=kTRUE,
            Bool_t autocreate=kTRUE,
            Bool_t ismaster=kTRUE);

      virtual ~TGo4ServerTask();

      /**
       * Quit method used by quit command; may be overridden
       * in special application
       */
      virtual void Quit();

      /**
       * Fast Quit() without waiting for proper disconnection.
       * For analysis server shutdown.
       */
      void Shutdown();

      TGo4TaskManager* GetTaskManager();

      /** Get task handler of the currently activated client connection */
      TGo4TaskHandler* GetCurrentTaskHandler();

      /** Get task handler for client specified by name */
      TGo4TaskHandler* GetTaskHandler();

      /** Get task handler for client specified by name */
      TGo4TaskHandler* GetTaskHandler(const char* name);

      TGo4BufferQueue* GetCommandQueue(const char* task=0);

      TGo4BufferQueue* GetStatusQueue(const char* task=0);

      TGo4BufferQueue* GetDataQueue(const char* task=0);

      /** Delivers next command. This implementation will poll over
       *  command queues of all existing connections. Command is labeled
       *  with name of ordering task, so data may be returned to
       *  command sender only.*/
      virtual TGo4Command* NextCommand();


      /** Send status object via status channel to the master. Master name
       * may be specified, if more than one master is connected.
       * By default, object is send to all connected tasks */
      virtual void SendStatus(TGo4Status * stat, const char* receiver=0);

      /** Send internal status buffer to the master(s). */
      virtual void SendStatusBuffer();

      /**
       * starts the thread that listens to the connector port
       * for a client negotiation request; used after launching an
       * rsh client from server
       */
      virtual Bool_t StartConnectorThread();

      /**
       * stops the thread that listens to the connector port
       * for a client negotiation request;
       * used before launching an rsh client from server.
       * A dummy connection is performed to release socket pending in listening
       * state, thus runnable may wait afterwards , and no
       * server socket is listening on spawning the client.
       * Tests showed that any child process took over the listening
       * server socket, which is not desired here and probably a bug of the system.
       */
      virtual Bool_t StopConnectorThread();

      /**
       * Returns the waiting state fbIsWaiting of the Connector
       * Thread;
       */
      Bool_t ConnectorThreadIsStopped();

      /**
       * sets current client task (i.e. taskhandler) by name
       */
      void SetCurrentTask(const char* name);

      void SetConnect(TGo4Socket * trans, const char* host, UInt_t port, Bool_t keepserv=kFALSE);

      void SetDisConnect(TGo4Socket * trans);

      TGo4Socket* GetConnectTransport();

      const char* GetConnectorName() const { return fxConnectorName.Data();}

      const char* GetConnectHost() const { return fxConnectHost.Data();}

      /**
       * this method is used by the connectortimer Notify to
       * connect or disconnect a transportchannel (TSocket) on demand;
       * the connection process itself is handled by the connector runnable
       * which uses services of the TaskManager class */
      virtual Int_t TimerConnect();

      /**
       * Remove all connected client task from this server.
       * with option force=true, remove without waiting
       */
      virtual Int_t RemoveAllClients(Bool_t force=false);

      /**
       * Remove the client task specified by name from this server.
       * If clientwait=true, remove with client handshake. If isterminating=true,
       * do not restart workthreads after removing the client.
       */
      virtual Bool_t RemoveClient(const char* name, Bool_t clientwait=kTRUE, Bool_t isterminating=kFALSE);

      /**
       * removes the currently active client from server (disconnect)
       */
      Bool_t RemoveCurrentClient();

      Int_t WaitForOpen();

      Int_t WaitForClose();

      Int_t WaitForConnection();

      static const char* Get_fgcLAUNCHPREFSFILE();

   protected:
      ////////////////////////////////////////////////////////////////////
      // public static constants:
      /** maximum cycles to wait until transport is open */
      static const Int_t fgiOPENWAITCYCLES;

      /** time for each open wait cycle */
      static const UInt_t fguOPENWAITCYCLETIME;

      /** maximum cycles to wait until transport is closed */
      static const Int_t fgiCLOSEWAITCYCLES;

      /** time for each close wait cycle */
      static const UInt_t fguCLOSEWAITCYCLETIME;

     /** maximum cycles to wait until transport is connected */
      static const Int_t fgiCONNECTWAITCYCLES;

      /** time for each connect wait cycle */
      static const UInt_t fguCONNECTWAITCYCLETIME;

      /** period of task connector timer */
      static const UInt_t fguCONNECTTIMERPERIOD;

      /** Name of the Preferences file for the client startup */
      static const char* fgcLAUNCHPREFSFILE;

private:

      /**
        * remember name of connector thread
        */
      TString fxConnectorName;

      /** @link aggregationByValue
             *   @supplierCardinality 1 */
      TGo4TaskManager * fxTaskManager;//!

      /** @supplierCardinality 1 */
      TGo4TaskHandler * fxCurrentTaskHandler; //!

      /**
       * link to the next TaskHandler transport instance that shall be connected
       * by the Application Control Timer with the client
       */
      TGo4Socket * fxConnectTransport; //!

      /**
       * link to the next TaskHandler transport instance that shall be disconnected
       * by the Application Control Timer with the client
       */
      TGo4Socket * fxDisConnectTransport; //!

      /**
       * hostname for timer connect
       */
      TString fxConnectHost;

      /**
       * port number for timer connect
       */
      UInt_t fuConnectPort;

      /**
       * True if open in server mode shall keep the server socket instance
       */
      Bool_t fbKeepServerSocket;

      /**
       * True if fxConnectTransport shall be Open() by AppControlTimer
       */
      Bool_t fbConnectRequest;

       /**
       * True if fxConnectTransport shall be Close() by AppControlTimer
       */
      Bool_t fbDisConnectRequest;


      /**
       * True if fxConnectTransport waits in server Open() call
       */
      Bool_t fbConnectIsOpen;

      /**
       * True if fxConnectTransport has returned from Open(),
       * i.e. connection was established
       */
      Bool_t fbConnectIsDone;

      /**
       * True if fxConnectTransport has returned from Close(),
       * i.e. connection was established
       */
      Bool_t fbConnectIsClose;
//!

      /** timer responsible for the connection/disconnection of clients;
       * independent of application control timer
       * @link aggregationByValue
       *   @supplierCardinality 1 */
      TGo4TaskConnectorTimer * fxConnectorTimer;
public:

      enum ELaunchmodes{
      kSecureShell    = BIT(0),     // kTRUE=launch client with ssh, otherwise rsh
      kGuiEmbed       = BIT(1)      // kTRUE=client terminial embedded in gui otherwise in Xterm
    };


ClassDef(TGo4ServerTask,1)
};

#endif //TGO4SERVERTASK_H
