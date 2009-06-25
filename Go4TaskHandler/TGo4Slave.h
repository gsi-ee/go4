#ifndef TGO4SLAVE_H
#define TGO4SLAVE_H

#include "TGo4TaskOwner.h"

class TGo4Status;
class TGo4TaskStatus;
class TGo4Command;
class TGo4ThreadHandler;
class TMutex;
/**
 * Baseclass for all slave process entities. A Slave receives commands and may send data and status objects via the TGo4Task connection. This can run either as server or client task.
 */
class TGo4Slave : public TGo4TaskOwner {
public:
    TGo4Slave() ;

    TGo4Slave(const char* name, Bool_t isserver=kTRUE, const char* serverhost="localhost", UInt_t negotport=5000) ;

    virtual ~TGo4Slave() ;

    /**
     * General start method of slave application to be called from remote
     * command; should be re-implemented by application
     */
    virtual void Start();

    /**
     * General stop method of slave application to be called from remote
     * command; should be re-implemented by application
     */
    virtual void Stop();

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

    virtual void TerminateFast ();

    /**
     * create a status object with information on the
     * current (slave) task. this object may be sent to the
     * master or may be used to restore current status by
     * memento mechanism
     */
    virtual TGo4TaskStatus* CreateStatus();

    Bool_t MainIsRunning() { return fbMainIsRunning; }

    /** Delivers next command from command queue. */
    TGo4Command* NextCommand();

    /** Send object via data channel to the server. */
    void SendObject(TObject * obj, const char* receiver=0);

    /** Send status object via status channel to the server. */
    void SendStatus(TGo4Status * stat, const char* receiver=0);


    /**
     * Overrides the ThreadManager Initialization; is used to setup the
     * client connections on demand; checks for fbConnectRequest flag
     */
    virtual Int_t Initialization();

    TGo4ThreadHandler* GetThreadHandler();

    /** Send internal status buffer to the server. */
    void SendStatusBuffer();

    /**
     * Send message string in a status object to the gui.
     * Will be displayed in status window. Level indicates info, warning or error,
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
     * Execute string command. Uses gInterpreter by default. May be overridden
     * to evaluate go4 command strings in subclass.
     */
    virtual void ExecuteString(const char* command);

protected:

    /**
     * method needed by method
     * CreateStatus of any status subclasses to
     * set the values of the clienttask specific
     * part of the status object.
     */
    virtual void UpdateStatus(TGo4TaskStatus* state);

private:

    /**
     * True if analysis main loop is running. If false, the slave main thread
     * will just wait on incoming commands without processing main loop.
     */
    Bool_t fbMainIsRunning;
};
#endif //TGO4SLAVE_H
