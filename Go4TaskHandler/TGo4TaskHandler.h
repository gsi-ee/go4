#ifndef TGO4TASKHANDLER_H
#define TGO4TASKHANDLER_H

#include "TNamed.h"
#include "TGo4Command.h"

class TGo4ThreadManager;
class TGo4ThreadHandler;
class TGo4Socket;
class TGo4Queue;
class TGo4CommandInvoker;
class TGo4TaskHandlerStatus;
class TGo4DataRunnable;
class TGo4StatusRunnable;
class TGo4CommandRunnable;


/**
 * This class is responsible for the interconnection of two tasks:
 *  provided are three communication channels (data, command, status),
 *  one thread runnable is acting on each transport channel (e.g. socket);
 *  one queue (buffer) is assigned to each channel
 *  depending on client or server mode of the task handler, the threads get objects
 *  from transporl and put them into the queue, or vice versa. (servertask sends commands,
 *   but receives data and status objects)
 * taskhander is named such that it can be found in a list (i.e. task manager) by the server task
 * which may serve more than one client tasks
 */
class TGo4TaskHandler : public TNamed {

  public:

    friend class TGo4TaskManager;

    TGo4TaskHandler(const char* name, TGo4ThreadManager* threadmanager, Bool_t clientmode=kFALSE, Bool_t mastermode=kTRUE,UInt_t negotiationport=0);

    virtual ~TGo4TaskHandler();

    /**
     * Method defining the connection protocol of
     * one data transport channel (data, status, or command).
     * For server socket mode (used by taskmanager)
     */
    Bool_t ConnectServerChannel(const char* name, TGo4Socket* negotiator, TGo4Socket* channel, const char* host);

    /**
     * method defining the connection protocol of
     * one data transport channel (data, status, or command).
     * For client socket mode (used by taskhandler)
     */
    Bool_t ConnectClientChannel(const char* name, TGo4Socket * negotiator, TGo4Socket * channel, const char* host);

    /**
     * request to server at host to connect or disconnect us, returns negotiation channel
     */
    TGo4Socket* ServerRequest(const char* host="localhost");

    /** Negotiate login to requested server channel with account type */
    Bool_t ServerLogin(TGo4Socket* connector, Go4CommandMode_t account);

    /**
     * establishes the connections of all three transport channels and starts the service threads
     */
    Bool_t Connect(const char* host="localhost", TGo4Socket* negotiator=0);


    /**
     * Closes the connections of all three transport channels.
     * Argument waitforclient specifies if server shall wait
     * for client handshake over negotiation channel
     * before shutting down the sockets.
     */
    Bool_t DisConnect(Bool_t waitforclient=kTRUE);

    TGo4ThreadManager* GetThreadManager() const { return fxThreadManager; }

    /**
     * Get actual local port number of specified
     * Go4 socket. Method polls until the port number is
     * not zero, i.e. the created server socket has been bound
     * to that number. Used by the Connect protocol to
     * find out server port before sending it to the client.
     */
    Int_t WaitGetPort(TGo4Socket* sock);

    /** Set port for the negotiation channel (client mode). Required
     * if connection should be done on different port after taskhandler was
     * created. */
    void SetNegotiationPort(UInt_t port){ fuNegPort=port;}

    TGo4Socket* GetCommandTransport() const { return fxCommandTransport; }

    TGo4Socket* GetStatusTransport() const { return fxStatusTransport; }

    TGo4Socket* GetDataTransport() const { return fxDataTransport; }

    TGo4Queue* GetCommandQueue() const { return fxCommandQueue; }

    TGo4Queue* GetStatusQueue() const { return fxStatusQueue; }

    TGo4Queue * GetDataQueue() const { return fxDataQueue; }

    const char* GetHostName() const { return fxHostName.Data(); }

    const char* GetComName() const { return fxComName.Data(); }

    const char* GetDatName() const { return fxDatName.Data(); }

    const char* GetStatName() const { return fxStatName.Data(); }

    Int_t GetComPort() const { return fiComPort; }

    Int_t GetDatPort() const { return fiDatPort; }

    Int_t GetStatPort() const { return fiStatPort; }

    Go4CommandMode_t GetRole(){return fiRole;}
    void SetRole(Go4CommandMode_t role){fiRole=role;}

    /**
     * returns pointer to copy of client invoker which may be used
     * by server to check the valid command clients
     */
    TGo4CommandInvoker* GetInvoker() const { return fxInvoker; }

    /** Check whether this instance is client or server taskhandler */
    Bool_t IsClientMode() const { return fbClientMode; }

    /** Check whether this instance is client or server taskhandler */
    Bool_t IsMasterMode() const { return fbMasterMode; }

    /**
     * sets the internal aborting state of this taskhandler instance;
     * will be checked before an exception tries to cancel this taskhandler
     */
    void SetAborting(Bool_t isaborting=kTRUE) { fbIsAborting=isaborting; }

    /** Check whether this instance is currently being aborted */
    Bool_t IsAborting() const { return fbIsAborting; }

    /**
     * sets pointer to client invoker which may be used
     * by server to check the valid command clients
     */
    void SetInvoker(TGo4CommandInvoker* inv) { fxInvoker=inv; }

    /**
     * create a status object with information on the
     * current taskhandler state. this object may be sent to the
     * server or may be used to restore current status by
     * memento mechanism
     */
    virtual TGo4TaskHandlerStatus * CreateStatus();

    /**
     * Stop all transport threads of this taskhandler (command,
     * data, status). If wait is true, wait until threads are really
     * stopped before return. If not all threads were stopped
     * within defined stoptime and waitcycles, return value is false.
     */
    Bool_t StopTransportThreads(Bool_t wait=kTRUE);

    /**
     * Start all transport threads of this taskhandler (command,
     * data, status).
     */
    void StartTransportThreads();

    /** Wait for the thread of name to be stopped. Returns
      * false if thread has not reached stopped state within
      * the defined stoptime and cycles. */
    Bool_t WaitThreadStop(const char* name);

   /** Close transport channels and clear queues.
     * Option may indicate root TSocket Close option, e.g.
     * "force" to force socket close */
    void CloseChannels(Option_t *opt="");

    /** Clear all entries of queues. */
    void ClearQueues();


  /** Specify login name and password for administrator account.
    * This is used when client requests for a server connection.
    * For multiple master clients at a slave server, there can be
    * only one controller, but many observers with restricted command
    * rights. */
   static void SetAdminAccount(const char* name, const char* passwd);

   /** Specify login name and password for controller account.
    * This is used when client requests for a server connection.
    * For multiple master clients at a slave server, there can be
    * only one controller. Master server at slave client has always
    * controller role. */
   static void SetCtrlAccount(const char* name, const char* passwd);

    /** Specify login name and password for observer account.
    * This is used when client requests for a server connection.
    * For multiple master clients at a slave server, there can be
    * only one controller, but many observers with restricted command
    * rights. */
   static void SetObservAccount(const char* name, const char* passwd);

   static const char* Get_fgcOK();

   static const char* Get_fgcERROR();

    static UInt_t Get_fguPORTWAITTIME();

    static Int_t Get_fgiPORTWAITCYCLES();

  protected:

    /** Default port number of negotiation connection (raw transport) */
    static const UInt_t fguCONNECTORPORT;        //!

    /** cycles to wait for taskhandler thread stop on disconnection */
    static const Int_t fgiTHREADSTOPCYCLES;

    /** time for each threadstop wait cycle */
    static const UInt_t fguTHREADSTOPTIME;

    /** Delay time (ms) to wait between two checks of transport connection */
    static const UInt_t fguTRANSPORTCHECKDELAY;  //!

    /** Maximum entries allowed for status queue */
   static const UInt_t  fguSTATUSQUEUESIZE;      //!

   /** Maximum entries allowed for data queue */
   static const UInt_t fguDATAQUEUESIZE;         //!

   /** Maximum entries allowed for command queue */
   static const UInt_t fguCOMMANDQUEUESIZE;      //!

    /** Initial string for connect request (raw transport) */
    static const Text_t fgcCONNECT[];            //!

    /** Initial string for disconnect request (raw transport) */
    static const Text_t fgcDISCONNECT[];         //!

  /** Task identifier for client connect negotiations (raw transport) */
    static const Text_t fgcMASTER[];              //!

  /** Task identifier for client connect negotiations (raw transport) */
    static const Text_t fgcSLAVE[];              //!


   /** Suffix for command thread name */
   static const Text_t fgcCOMMANDTHREAD[];       //!

   /** Suffix for status thread name */
   static const Text_t fgcSTATUSTHREAD[];        //!

   /** Suffix for data thread name */
   static const Text_t fgcDATATHREAD[];          //!


  private:
    TGo4TaskHandler();


/** This keeps account for admin connection.
  * Name is accountname, title is password. May be set from outside by
  * public methods SetObservAccount and SetObservPasswd.
  * Later we might keep this with encryption...*/
    static TNamed fgxADMINISTRATORACCOUNT;


/** This keeps account for observer connection.
  * Name is accountname, title is password. May be set from outside by
  * public methods SetObservAccount and SetObservPasswd.
  * Later we might keep this with encryption...*/
    static TNamed fgxOBSERVERACCOUNT;

/** This keeps account for controller connection.
  * Name is accountname, title is password. May be set from outside by
  * public methods SetCtrlAccount and SetCtrlPasswd.
  * Later we might keep this with encryption...*/
    static TNamed fgxCONTROLLERACCOUNT;

    /** Error string for client connect negotiations (raw transport) */
    static const char* fgcERROR;              //!

    /** Ok string for client connect negotiations (raw transport) */
    static const char* fgcOK;                 //!

    /** Time for each waitgetport cycle */
    static const UInt_t fguPORTWAITTIME;         //!

    /** Cycles to wait for a nonzero portnumber */
    static const Int_t fgiPORTWAITCYCLES;        //!

   /** True if taskhandler runs in client mode */
    Bool_t fbClientMode;

   /** True if taskhandler runs in master mode (sends commands, receives data and status). Otherwise false.*/
    Bool_t fbMasterMode;

    /** True if taskhandler is being aborted e.g. by remove exception */
    Bool_t fbIsAborting;

   /** Link to threadmanager that owns the task handler, used for runnable ctors */
    TGo4ThreadManager* fxThreadManager; //!

    /**
     * link to external threadhandler which is used for the transport service threads (runnables);
     * these runnables are instantiated from the task handler ctor and passed to the thread handler who takes
     * over the responsibility for the runnables (threadhandler internal mode threads)
     * @associates <{TGo4ThreadHandler}>
     */
    TGo4ThreadHandler* fxThreadHandler; //!

    /** Link to external command invoker instance used for direct command in client mode: */
    TGo4CommandInvoker* fxInvoker; //!

    /** Buffers (queues) for the three transport channels: */
    TGo4Queue* fxCommandQueue; //!
    TGo4Queue* fxStatusQueue; //!
    TGo4Queue* fxDataQueue; //!

    /** Transport channels (e.g. sockets) for command, status, data */
    TGo4Socket* fxCommandTransport; //!
    TGo4Socket* fxStatusTransport;  //!
    TGo4Socket* fxDataTransport;  //!

    /** Remember name of command thread */
    TString fxComName;

    /** Remember name of status thread */
    TString fxStatName;

    /** Remember name of data thread */
    TString fxDatName;

    /** Remember name of remote task handler */
    TString fxHostName;

    /** Actual port number of negotiation connection (raw transport) */
    UInt_t fuNegPort;

    /** Command connection port number */
    Int_t fiComPort;

    /** Status connection port number */
    Int_t fiStatPort;

    /** Data connection port number */
    Int_t fiDatPort;

    /** For server connection to client: This indicates role of
     * connected client, if client is master. Depending on role,
     * some actions may be forbidden. Role is equivalent to
     * allowed command execution mode. */
    Go4CommandMode_t fiRole;

    /** link to status runnable which is managed by thread handler
     * @supplierCardinality 1
     * @clientCardinality 1*/
    TGo4StatusRunnable* fxStatusRun; //!

    /** link to data runnable which is managed by thread handler
     * @supplierCardinality 1
     * @clientCardinality 1*/
    TGo4DataRunnable* fxDataRun; //!

    /** link to command runnable which is managed by thread handler
     * @supplierCardinality 1
     * @clientCardinality 1*/
    TGo4CommandRunnable* fxCommandRun; //!

};

#endif //TGO4TASKHANDLER_H
