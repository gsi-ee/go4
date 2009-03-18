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
