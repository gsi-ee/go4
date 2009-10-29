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

#ifndef TGO4EXAMPLESERVER_H
#define TGO4EXAMPLESERVER_H

#include "TGo4ServerTask.h"

#include "TGo4ExampleController.h"

/**
 * example class for a server task that fits to the example application
 */

class TGo4ExampleServer : public TGo4ServerTask {

  public:

    TGo4ExampleServer(const char* name,
                      UInt_t negotiationport=0,
                      Bool_t blockingmode=kFALSE);

    virtual ~TGo4ExampleServer();

    TGo4ExampleController* GetController();

    TGo4TaskHandlerCommandList* CreateCommandList();

    virtual Int_t StartWorkThreads();

    virtual Int_t StopWorkThreads();

    /** suffix for controller thread name */
    static const char* fgcCONTROLTHREAD; //!

    /** suffix for logging thread name */
    static const char* fgcLOGGINGTHREAD;//!

  private:

    TGo4ExampleServer();

    /** @link aggregationByValue
     *   @supplierCardinality 1
     * @clientCardinality 0..1*/
    TGo4ExampleController * fxController;//!

    /** remember name of controller thread */
    TString fcControlName; //!

    /** remember name of logging thread */
    TString fcLoggingName; //!

  ClassDef(TGo4ExampleServer,1)
};

#endif //TGO4EXAMPLESERVER_H
