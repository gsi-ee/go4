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

#ifndef TGO4ANALYSISWATCHRUNNABLE_H
#define TGO4ANALYSISWATCHRUNNABLE_H

#include "TGo4Runnable.h"

class TGo4Analysis;
class TGo4AnalysisClient;

/**
 * runnable performing control watcher functions of analysis client task:
 * status of analysis is send to status queue
 * @stereotype thread
 * @author J. Adamczewski
 * @since 11-Jan-2001
 */
class TGo4AnalysisWatchRunnable : public TGo4Runnable {
  public:
     TGo4AnalysisWatchRunnable(const char* name, TGo4AnalysisClient* cli);

     virtual ~TGo4AnalysisWatchRunnable();

     virtual Int_t Run(void*);

    /** frequency in ms for the watcher runnable to check the application */
    static const UInt_t fguWATCHINTERVAL;        //!

  private:

    /** @supplierCardinality 1
     * @directed
     * @clientCardinality 0..1*/
    TGo4Analysis * fxAnalysis;                   //!

  /** @supplierCardinality 1
      * @directed
      * @clientCardinality 0..1*/
    TGo4AnalysisClient * fxAnalysisClient;                   //!


    TGo4AnalysisWatchRunnable();
};

#endif //TGO4ANALYSISWATCHRUNNABLE_H
