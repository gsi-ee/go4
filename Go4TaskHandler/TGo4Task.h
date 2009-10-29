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

#ifndef TGO4TASK_H
#define TGO4TASK_H

#include "TGo4ThreadManager.h"
#include "TGo4CommandReceiver.h"
#include "Go4EmergencyCommands.h"

#include "TString.h"

class TMutex;
class TGo4TaskOwner;
class TGo4Slave;
class TGo4Master;
class TGo4BufferQueue;
class TGo4ObjectQueue;
class TGo4TaskHandler;
class TGo4TaskHandlerCommandList;
class TGo4CommandProtoList;
class TGo4Command;
class TGo4Status;
class TGo4TaskStatus;

class TGo4Task : public TGo4ThreadManager, public TGo4CommandReceiver {

public:
    friend class TGo4LocalCommandRunnable; // access to command queue
    friend class TGo4Slave; // access to UpdateStatus
    friend class TGo4TaskOwner; // set workisstopped flag

    TGo4Task(const char* name, Bool_t blockingmode,
                               Bool_t autostart=kFALSE,
                               Bool_t autocreate=kTRUE,
                               Bool_t ismaster=kFALSE);

    virtual ~TGo4Task();

    /**
     * General start method of client application to be called from remote
     * command; should be re-implemented by application
     */
    virtual void Start();

    /**
     * General stop method of client application to be called from remote
     * command; should be re-implemented by application
     */
    virtual void Stop();

    /**
     * Quit the client; method to be called from
     * command object, may be overridden in user
     * implementation
     */
    virtual void Quit();

    /**
     * Kill the main thread; method to be called from
     * command, should be overridden in user
     * implementation
     */
    virtual void KillMain();

    /**
     * Restart the main thread; method to be called from
     * command, should be overridden in user
     * implementation
     */
    virtual void RestartMain();

    virtual void Terminate (Bool_t termapp=kTRUE);

    /** Overwrites the Threadmanager TerminateFast to shutdown the objectserver properly */
    virtual void TerminateFast ();

    /**
     * Execute string command. Uses gInterpreter by default. May be overridden
     * to evaluate go4 command strings in subclass.
     */
    virtual void ExecuteString(const char* command);

    Bool_t IsMaster() { return fbCommandMaster; }

    /** Set owner object and switch automatically in master or slave
     *  mode, depending on owner type */
    void SetOwner(TGo4TaskOwner* owner);

    virtual TGo4TaskHandler* GetTaskHandler();

    virtual TGo4BufferQueue* GetCommandQueue(const char* task=0);

    virtual TGo4BufferQueue* GetStatusQueue(const char* task=0);

    virtual TGo4BufferQueue* GetDataQueue(const char* task=0);

   /**
     * Add a user command object to the existing command list. This can be done on the fly.
     * Command receiver name of user command must match "Analysis", "AnalysisClient",
     * or the name of the user analysis to be executed on the remote side.
     */
    void AddUserCommand(TGo4Command* com);

    /**
     * Add a user command list to the existing command list. This can be done on the fly.
     * Commands of the user list are added to the existing command list.
     */
    void AddUserCommandList(TGo4CommandProtoList * comlist);

    /**
     * Delivers next status object  from status queue.
     * Encapsulates the queue wait and the object streaming.
     * Streamer is decoupled from queue wait and sets global lockguard.
     * If wait=kFALSE, queue will be polled once instead of condition wait.
     */
    TGo4Status* NextStatus(Bool_t wait=kTRUE);

    /**
     * Delivers next object  from data queue.
     * Encapsulates the queue wait and the object streaming.
     * Streamer is decoupled from queue wait and sets global lockguard.
     * If wait=kFALSE, queue will be polled once instead of condition wait.
     */
    TObject * NextObject(Bool_t wait=kTRUE);

    /** Delivers next command from command queue. */
    virtual TGo4Command* NextCommand();

    /** Send object via data channel to the master. Master name
     * may be specified, if more than one master is connected.
     * By default, object is send to current active task */
    void SendObject(TObject * obj, const char* receiver=0);

    /** Send status object via status channel to the master. Master name
     * may be specified, if more than one master is connected.
     * By default, object is send to all connected tasks */
    virtual void SendStatus(TGo4Status * stat, const char* receiver=0);

    /** Send internal status buffer to the master(s). */
    virtual void SendStatusBuffer();

    /** Access to the mutex of status buffer. To be locked
     * outside any command, otherwise deadlocking possible*/
    TMutex* GetStatusBufferMutex(){return fxStatusMutex;}

    /**
     * Send message string in a status object to the gui.
     * Will be displayed in status window. Level indicates info, warning or error.
     * Message text may begin with name of receiver for message, followed by "::".
     * If no receiver is defined, all connected masters will receive message (default).
     */
    void SendStatusMessage(Int_t level, Bool_t printout, const char* text, ...);

    /**
     * Create status object and stream it into the
     * status send buffer. To be executed from
     * the main thread; buffer will be send by
     * status thread.
     */
    void UpdateStatusBuffer();

    /**
     * Overrides the ThreadManager Initialization; is used to setup the
     * client connections on demand; checks for fbConnectRequest flag
     */
    virtual Int_t Initialization();

    /**
     * create a status object with information on the
     * current (slave) task. this object may be sent to the
     * master or may be used to restore current status by
     * memento mechanism
     */
    virtual TGo4TaskStatus* CreateStatus();

    /**
     * send command specified by name to the current client task
     */
    Bool_t SubmitCommand(const char* name);

    /**
     * send emergency quit command to the current client task
     */
    Bool_t SubmitEmergencyCommand(Go4EmergencyCommand_t val);

    /**
     * Send emergency command via data channel. For analysis server
     * shutdown, to inform clients about disconnect
     */
    Bool_t SubmitEmergencyData(Go4EmergencyCommand_t val, const char* receiver=0);


    /** Send given command to the current client task. Command object is deleted after
      * this call internally! */
    Bool_t SubmitLocalCommand(TGo4Command* com);

    /** send given command to the current client task. Command object is deleted after
      * this call internally! */
    Bool_t SubmitCommand(TGo4Command* com);

    /**
     * Method using internal command prototype list to produce a clone
     * of the specified command
     */
    TGo4Command* MakeCommand(const char* name);

    /**
     * Put dummy object into command queue
     * to wake up user threads which might
     * wait for a command. To be used in
     * quit and restart methods. Id can set in such way that
     * application can check the command id for special action,
     * e.g. stop thread in case of termination.
     */
    void WakeCommandQueue(Int_t id=0);

    /**
     * start the working threads of the task implementation;
     * this method is used after the current connection has changed,
     * to continue reading/writing
     * the queues. To be overridden by the derived task.
     */
    virtual Int_t StartWorkThreads();

    /**
     * stop the working threads of the task implementation;
     * this method is used before the current connection is changed to prevent reading/writing
     * and waiting for the wrong queues. To be overridden by the derived task.
     */
    virtual Int_t StopWorkThreads();

    Bool_t IsWorkStopped(){return fbWorkIsStopped;}

  /** Lock all go4 mutexes in correct order to avoid deadlocking.
     * to be used from cintlocktimer */
    virtual void LockAll();

    /** UnLock all go4 mutexes in correct order to avoid deadlocking
    * to be used from cintlocktimer */
    virtual void UnLockAll();


    /**
     * Send current status information of the task to the
     * master; may be re-implemented by application subclass
     * of TGo4Task
     */
    virtual void GetStatus();

    const char* GetCommanderName() const { return fxCommanderName.Data(); }

    TGo4Master* GetMaster() { return fxMaster; }

    TGo4Slave* GetSlave() { return fxSlave; }

    /**
     * Send message buffers with stop values into queues and
     * via socket connections. Used in disconnect procedure
     * to put taskhandler threads into suspend mode, i.e.
     * wake them from socket receive or queue wait. For multiple
     * client connection, taskname defines which client to stop.
     */
    void SendStopBuffers(const char* taskname=0);

    /** Use preallocated abort buffer to stop remote socket wait
     * from taskhandler itself */
    TBuffer* GetAbortBuffer(){return fxAbortBuffer;}

    static Int_t Get_fgiTERMID();

  protected:

    /** factory method for command list; overridden by implementation */
    virtual TGo4TaskHandlerCommandList*  CreateCommandList();

    /** get pointer to currently valid command list */
    TGo4TaskHandlerCommandList * GetPrototype();

    /**
     * method needed by method
     * CreateStatus of any status subclasses to
     * set the values of the clienttask specific
     * part of the status object.
     */
    virtual void UpdateStatus(TGo4TaskStatus* state);

    TGo4ObjectQueue* GetLocalCommandQueue() { return fxLocalCommandQueue; }

    void SetMaster(Bool_t on=kTRUE) { fbCommandMaster=on; }

    void SetWorkIsStopped(Bool_t on) { fbWorkIsStopped=on; }

    /** Mutex protecting status buffer between main and  watch thread. */
    TMutex * fxStatusMutex; //!

    /** Buffer containing the analysis status which is  updated by the main thread */
    TBuffer* fxStatusBuffer; //!

  private:

    void SetMaster(TGo4Master* m);

    void SetSlave(TGo4Slave* s);

    /** the command list which holds all command prototypes */
    TGo4TaskHandlerCommandList * fxCommandPrototype; //!

    /** queue for the server side local commands, submitted by the gui or cli */
    TGo4ObjectQueue* fxLocalCommandQueue; //!

    /** This flag indicates the main state of the task: either master
      * (sends commands, receives data and status), or slave
      * (receives commands and may send date and status)  */
    Bool_t fbCommandMaster;

    /** remember name of localcommand thread */
    TString fxCommanderName;

    /** @supplierCardinality 0..1*/
    TGo4Master * fxMaster; //!

    /** @supplierCardinality 0..1*/
    TGo4Slave * fxSlave; //!

    /** @supplierCardinality 0..1*/
    TGo4TaskOwner * fxOwner;//!

    /** True if user work threads are stopped for connection change */
    Bool_t fbWorkIsStopped;


    /** this buffer is used for stop message for threads */
    TBuffer* fxStopBuffer; //!

     /** this buffer is used for quit command  */
    TBuffer* fxQuitBuffer; //!

     /** this buffer is used for emergeny socket aborting  */
    TBuffer* fxAbortBuffer; //!

    /** Id number of dummy command that wakes the command queue on termination */
    static const Int_t fgiTERMID;


  ClassDef(TGo4Task,1)
};

#endif //TGO4TASK_H
