#ifndef TGO4ANALYSISMAINRUNNABLE_H
#define TGO4ANALYSISMAINRUNNABLE_H

#include "TGo4Runnable.h"

class TGo4AnalysisClient;
class TGo4Analysis;

/**
 * Main Runnable of the go4 analysis. Takes commands out of
 * command queue and processes them, then acts on the
 * analysis framework MainCycle method to process the analysis.
 * @since 11-Jan-2001
 * @stereotype thread
 * @author J. Adamczewski
 */
class TGo4AnalysisMainRunnable : public TGo4Runnable {
  public:

    TGo4AnalysisMainRunnable(const char* name, TGo4AnalysisClient* cli);

    virtual ~TGo4AnalysisMainRunnable();

    virtual Int_t Run(void*);

    virtual Int_t PreRun(void * );

    virtual Int_t PostRun(void * );

    /** frequency in ms for polling the run flag
      * (server mode without command queue) */
    static const UInt_t fguPOLLINTERVAL;

  private:
    /** @supplierCardinality 1
      * @directed
      * @clientCardinality 0..1*/
    TGo4Analysis * fxAnalysis;                   //!

    /** @supplierCardinality 1
      * @directed
      * @clientCardinality 0..1*/
    TGo4AnalysisClient * fxAnalysisClient;                   //!

    TGo4AnalysisMainRunnable();
};

#endif //TGO4ANALYSISMAINRUNNABLE_H
