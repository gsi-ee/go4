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

#ifndef TGO4ANALYSISCLIENTIMP_H
#define TGO4ANALYSISCLIENTIMP_H

#include "TGo4Slave.h"

#include "TString.h"

class TTimer;
class TGo4Analysis;
class TGo4ClientStatus;
class TGo4Ratemeter;
class TGo4HistogramServer;
class TGo4TaskStatus;
class TGo4InterruptHandler;

/**
 * @author J. Adamczewski
 * @since 12/2000, last 5/2005
 */

class TGo4AnalysisClient : public TGo4Slave {

  public:
    TGo4AnalysisClient(const char* name,
                       TGo4Analysis* analysis,
                       const char* host="localhost",
                       UInt_t negport=0,
                       Bool_t histoserver=kFALSE,
                       const char* basename="Go4",
                       const char* passwd="abcd",
                       Bool_t servermode=kFALSE,
                       Bool_t autorun=kFALSE,
                       Bool_t cintmode = kFALSE);

    TGo4AnalysisClient(int argc, char** argv,
                       TGo4Analysis* analysis,
                       Bool_t histoserver=kFALSE,
                       const char* basename="Go4",
                       const char* passwd="abcd",
                       Bool_t servermode=kFALSE,
                       Bool_t autorun=kFALSE);

    virtual ~TGo4AnalysisClient();

    TGo4Analysis* GetAnalysis() const { return fxAnalysis; }

    virtual void Start();

    virtual void Stop();

    /** Kill the main thread; method to be called from
      * command, should be overridden in user implementation */
    virtual void KillMain();

    /** Restart the main thread; method to be called from
      * command, should be overridden in user implementation */
    virtual void RestartMain();

    /** create a status object with information on the
      * current client state. this object may be sent to the
      * server or may be used to restore current status by
      * memento mechanism. Overridden from superclass */
    virtual TGo4TaskStatus* CreateStatus();

    /** Override the ClientTask/ThreadManager  Initialization used by AppControlTimer
      * to init event classes before the threads are started. */
    virtual Int_t Initialization();

    /** Fetch object of name from analysis instance and put
      * it into the data queue of the client. Send an error status
      * object via status queue if there is no such object. */
    void SendAnalysisObject(const char* name);

    /** Send a complete status object of the analysis via
      * the status queue. */
    void SendAnalysisStatus();

    /** Send a complete status object of the analysis client via
      * the status queue. */
    void SendAnalysisClientStatus();

    /** Send the list of object names (folder structure) as status object to the gui. */
    void SendNamesList();

    /** Update ratemeter. To be used from main thread. */
    void UpdateRate(Int_t counts=1);

    /** Number of currently processed events since last start. */
    UInt_t GetCurrentCount();

    /** Test if ratemeter has been updated. */
    Bool_t TestRatemeter();

    /** Test if status buffer shall be updated now */
    Bool_t TestBufferUpdateConditions();

    /** Start the object (histogram) server. Existing server will be stopped and replaced
      * by new server with different basename and password. */
    void StartObjectServer(const char* basename,  const char* passwd);

    /** Stop the object (histogram) server. */
    void StopObjectServer();


    /**
     * start the working threads of the slave implementation;
     */
    virtual Int_t StartWorkThreads();

    /**
     * stop the working threads of the slave implementation;
     */
    virtual Int_t StopWorkThreads();

    /** Overwrites the Threadmanager Terminate to shutdown the objectserver properly */
    virtual void Terminate (Bool_t termapp=kTRUE);

    /** Overwrites the Threadmanager TerminateFast to shutdown the objectserver properly */
    virtual void TerminateFast ();

    /** Execute string command. Overrides base class implementation
      * to decouple some commands from analysis client. */
    virtual void ExecuteString(const char* command);

    void SetCintMode(Bool_t on=kTRUE);

    Bool_t IsCintMode() const { return fbCintMode; }

    /** Lock all go4 mutexes in correct order to avoid deadlocking.
     * to be used from cintlocktimer */
    void LockAll();

    /** UnLock all go4 mutexes in correct order to avoid deadlocking
    * to be used from cintlocktimer */
    void UnLockAll();

    /** suffix for main thread name */
    static const char* fgcMAINTHREAD; //!

    /** suffix for watch thread name */
    static const char* fgcWATCHTHREAD; //!

    /** Number of mainloop runs between two updates of the client status object. */
    static const UInt_t fguSTATUSUPDATE;

    /** Maximum time (in s) between two updates of the client status object. */
    static const Double_t fgdSTATUSTIMEOUT;

    /** Timer period (in ms) for cint locking timer*/
    static const UInt_t fguCINTTIMERPERIOD;

  private:

    void Constructor(Bool_t starthistserv=kFALSE, const char* basename="Go4", const char* passwd="abcd");

    /** Method needed by method CreateStatus of any status subclasses to
      * set the values of the clienttask specific part of the status object. */
    void UpdateStatus(TGo4TaskStatus * state);



private:

    /** The analysis the client is working on.
      * @link aggregationByValue
      * @supplierCardinality 1
      * @clientCardinality 0..1*/
    TGo4Analysis* fxAnalysis;                   //!

    /** remember name of main thread */
    TString fcMainName;                          //!

    /** remember name of watch thread */
    TString fcWatchName;                         //!

    /** absoulute time of the last status buffer update. Reset by TestBufferUpdateCondition */
    Double_t fdBufferUpdateTime;

    /** @link aggregation
      *   @supplierCardinality 1 */
    TGo4Ratemeter* fxRatemeter;                  //!

    /** @link aggregation
      *   @supplierCardinality 1
      * histogram server (optional) */
    TGo4HistogramServer* fxHistoServer;          //!

     /** If true, this slave will initialize and start analysis
      * immediately without waiting for a submit/start command. */
    Bool_t fbAutoStart;

    /** If true, this slave runs to control CINT macro.
      * Will have effect on some initialization behaviour. */
    Bool_t fbCintMode;

    /** For cintmode, we only allow to release the main
     * lock within a certain time window, triggered by this
     * timer */
    TTimer* fxCintLockTimer; //!

    TGo4AnalysisClient();

  ClassDef(TGo4AnalysisClient,1)

};

#endif //TGO4ANALYSISCLIENT_H
