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

#ifndef TGO4TASKMANAGER_H
#define TGO4TASKMANAGER_H

#include "TNamed.h"
#include "TGo4Command.h"

class TObjArray;
class TMutex;
class TGo4ServerTask;
class TGo4Socket;
class TGo4TaskHandler;
class TIterator;

class TGo4TaskManager : public TNamed {
   public:

      TGo4TaskManager(const char* name, TGo4ServerTask * server, UInt_t negotiationport=0, Bool_t createconnector=kTRUE);

      virtual ~TGo4TaskManager();

      /**
       * used by connector runnable to wait for a client connect/disonnect request
       */
      Int_t ServeClient();

      /** Check account and password of the client that requests a
       * connection to this server. Return value specifies
       * client access priorities as defined in TGo4Command.h
       * Returns 0 in case of incorrect login. */
      Go4CommandMode_t ClientLogin();

      /**
       * used by connector runnable to wait for a client request to
       * connect to this server task. May specify mode of allowed commands
       * in case of client is master.
       */
      Int_t ConnectClient(const char* client, const char* host, Go4CommandMode_t role=kGo4ComModeController);

      /**
       * disonnect an existing client by name, wait for negotiation OK
       * if clientwait is true
       */
      Int_t DisConnectClient(const char* name , Bool_t clientwait=kTRUE);

      /**
       * disonnect an existing client by name, wait for negotiation OK
       * if clientwait is true
       */
      Int_t DisConnectClient(TGo4TaskHandler * taskhandler, Bool_t clientwait=kTRUE);

     /**
      * adds client task of name to manager: create server task handler and try to connect to client
      * Command execution role depends on previous login passowrd
      */
      Bool_t AddClient(const char* client, const char* host, Go4CommandMode_t role);

     /**
      * adds external task handler to array
      */
      Bool_t AddTaskHandler(TGo4TaskHandler* han);

      /**
      * creates new task handler with given name and adds it into array
      */
      TGo4TaskHandler* NewTaskHandler(const char* name);

      /**
      * removes task handler from array by name
      */
      Bool_t RemoveTaskHandler(const char* name);

      /**
      * returns certain task handler by name
      */
      TGo4TaskHandler* GetTaskHandler(const char* name);

      /**
      * returns last task handler in list
      */
      TGo4TaskHandler* GetLastTaskHandler();

     /**
      * For iteration over all connected task handlers. If
      * reset is true, internal iterator will be reset. Otherwies,
      * continue iterating list until last entry. Returns 0 when
      * iteration finishes.
      */
      TGo4TaskHandler* NextTaskHandler(Bool_t reset=kFALSE);

      TMutex* GetMutex(){return fxListMutex;}

      /** Number of entries in taskhandler list, i.e. number of connected clients */
      UInt_t GetTaskCount() const { return fuTaskCount; }

      /**
       * returns the portnumber for client server
       * negotiation port which is actually used by
       * the running taskmanager
       */
      UInt_t GetNegotiationPort();


     Int_t WaitForClientRemoved();

     Bool_t HasControllerTask(){return fbHasControllerConnection;}

      /**
       * cycles to wait for client disconnection
       */
      static const Int_t fgiDISCONCYCLES;

      /**
       * time for each disonnection wait cycle
       */
      static const UInt_t fguDISCONTIME;
   private:
      // do not use this ctors and assignment operators:

      TGo4TaskManager();

      TGo4TaskManager(const TGo4TaskManager &right);

      // private members:

      TGo4ServerTask* fxServer; //!

      TGo4Socket* fxTransport; //!

      /** @link aggregation
       *  @supplierCardinality 1..n */
      TObjArray* fxTaskList;    //!

      TMutex* fxListMutex;      //!

      /** Iterator over list of tasks.*/
      TIterator* fxTaskIter; //!

      /** @link aggregation
       * @directed
       * @supplierCardinality 1..*
       * @clientCardinality 1
       * @label TObjArray*/
      /*#  TGo4TaskHandler lnkTGo4TaskHandler1; */
      UInt_t fuTaskCount;

      /**
       * port number for the server client negotiation connections
       */
      UInt_t fuNegotiationPort;

      /**
       * True if the last specified client is removed from server.
       * flag is set by connector thread who performs the DisconnectClient
       * of TaskManager; is reset by WaitForClientRemoved method.
       */
      Bool_t fbClientIsRemoved;

      /** true if this server already has one connection to a
       * master client that has the controller role. Only one
       * controller or administrator taskhandler is allowed per server. */
      Bool_t fbHasControllerConnection;
};

#endif //TGO4TASKMANAGER_H

