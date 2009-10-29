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

#ifndef TGO4COMSETANALYSISSTATUS_H
#define TGO4COMSETANALYSISSTATUS_H

#include "TGo4AnalysisCommand.h"
#include "TGo4AnalysisStatus.h"

/**
 * Set the analysis framework to the status encapsulated in
 * this command. Analysis will be closed before the new
 * parameters are applied. This command does _not_ initialize
 * the event classes.
 */
class TGo4ComSetAnalysisStatus : public TGo4AnalysisCommand {
  public:

    TGo4ComSetAnalysisStatus();

    TGo4ComSetAnalysisStatus(TGo4AnalysisStatus* settings);

    virtual ~TGo4ComSetAnalysisStatus();

    Int_t ExeCom();

    /** Set internals of this command from specification of external
     * remote command (command container). Must be overloaded
     * for each specific command! */
    virtual void Set(TGo4RemoteCommand* remcon);


    void SetStatusObject(TGo4AnalysisStatus* settings);

  private:

    /** @link aggregationByValue
           *   @supplierCardinality 1 */
    TGo4AnalysisStatus * fxAnalysisStatus;

    ClassDef(TGo4ComSetAnalysisStatus,1)
};

#endif //TGO4COMSETANALYSISSTATUS_H
