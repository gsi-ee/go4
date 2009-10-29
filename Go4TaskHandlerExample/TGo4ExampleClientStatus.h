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

#ifndef TGO4EXAMPLECLIENTSTATUS_H
#define TGO4EXAMPLECLIENTSTATUS_H

#include "TGo4ClientStatus.h"
#include "TString.h"

class TH1;

class TGo4ExampleClientStatus : public TGo4ClientStatus {

  friend class TGo4ExampleClient;
  friend class TGo4ExampleDisplay;

  public:

     TGo4ExampleClientStatus() {}; // default ctor for streamer

     virtual ~TGo4ExampleClientStatus();

     /**
      * basic method to printout status information
      * on stdout; to be overridden by specific
      * subclass
      */
     virtual Int_t PrintStatus(Text_t* buffer=0, Int_t buflen=0);

  private:

     /** Remember name of main thread */
     TString fxMainName;

     /** Remember name of watch thread */
     TString fxWatchName;

     /** Remember name of histogram */
     TString fxHistogramName;

     /** Set names of the work runnables of example */
     void SetNames(const char* main, const char* watch);

     /** Set histogram status */
     void SetHistoStatus(TH1* histogram);

     TGo4ExampleClientStatus(const char* name);

  ClassDef(TGo4ExampleClientStatus,1)
};

#endif //TGO4EXAMPLECLIENTSTATUS_H
