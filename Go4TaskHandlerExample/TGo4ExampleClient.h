#ifndef TGO4EXAMPLECLIENT_H
#define TGO4EXAMPLECLIENT_H

#include "TGo4ClientTask.h"

class TGo4TaskStatus;
class TGo4ExampleApplication;
class TGo4ClientStatus;

/**
 * Example class for a client application;
 * this may  be the analysis client later on which keeps the
 * analysis class itself as aggregate and provides threads to work on analysis
 */

class TGo4ExampleClient : public TGo4ClientTask {
  public:

   TGo4ExampleClient(const char* name,
                     const char* host="localhost",
                     UInt_t negport=0,
                     Bool_t blockingmode=kTRUE);

   virtual ~TGo4ExampleClient();

   TGo4ExampleApplication* GetApplication();

   virtual void Start();

   virtual void Stop();

   /** Suffix for main thread name */
   static const Text_t fgcMAINTHREAD[]; //!

   /** Suffix for watch thread name */
   static const Text_t fgcWATCHTHREAD[];  //!

  protected:

    /**
     * method needed by method
     * CreateStatus of any status subclasses to
     * set the values of the clienttask specific
     * part of the status object.
     */
    virtual void UpdateStatus(TGo4ClientStatus* state);

  public:
    /**
     * create a status object with information on the
     * current client state. this object may be sent to the
     * server or may be used to restore current status by
     * memento mechanism. Overridden from superclass
     */
    virtual TGo4TaskStatus * CreateStatus();

  private:

    TGo4ExampleClient();

    /** @link aggregationByValue
     *   @supplierCardinality 1
     * @clientCardinality 0..1*/
    TGo4ExampleApplication* fxApplication;//!

    /** Remember name of main thread */
    TString fcMainName; //!

    /** Remember name of watch thread */
    TString fcWatchName; //!

  ClassDef(TGo4ExampleClient,1)
};

#endif //TGO4EXAMPLECLIENT_H
