#ifndef TGO4EXAMPLEAPPLICATION_H
#define TGO4EXAMPLEAPPLICATION_H

#include "TNamed.h"
#include "TH1.h"

#include "TGo4CommandReceiver.h"
#include "TGo4BufferQueue.h"

/**
 * Example application class; this class  may also run as standalone application
 * without the threaded client-server mechanism (e.g. as later analysis class)
 */
class TGo4ExampleApplication : public TGo4CommandReceiver, public TNamed {

  public:

    TGo4ExampleApplication(TGo4BufferQueue* datq=0);

    virtual ~TGo4ExampleApplication();

    void CommandAction1();

    void CommandAction2();

    void FillHistograms();

    TH1* GetHistogram();

    /**
     * put named data object into the current data queue;
     * will be sent to the gui server
     */
    void SendData(TNamed* data);

  private:

    TGo4BufferQueue * fxDataQ; //!

    TH1D* fxDemoHistogram;

};
#endif //TGO4EXAMPLEAPPLICATION_H



