// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

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

    TGo4AnalysisMainRunnable(const char *name, TGo4AnalysisClient* cli);

    virtual ~TGo4AnalysisMainRunnable();

    Int_t Run(void*) override;

    Int_t PreRun(void *) override;

    Int_t PostRun(void *) override;

    /** frequency in ms for polling the run flag
      * (server mode without command queue) */
    static const UInt_t fguPOLLINTERVAL;

  private:
    /** @supplierCardinality 1
      * @directed
      * @clientCardinality 0..1*/
    TGo4Analysis *fxAnalysis{nullptr};                   //!

    /** @supplierCardinality 1
      * @directed
      * @clientCardinality 0..1*/
    TGo4AnalysisClient * fxAnalysisClient{nullptr};                   //!

    TGo4AnalysisMainRunnable();
};

#endif //TGO4ANALYSISMAINRUNNABLE_H
