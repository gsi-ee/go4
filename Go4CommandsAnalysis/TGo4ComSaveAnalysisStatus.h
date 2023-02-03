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

#ifndef TGO4COMSAVEANALYSISSTATUS_H
#define TGO4COMSAVEANALYSISSTATUS_H

#include "TGo4AnalysisCommand.h"
#include "TString.h"

/**
 * Save current  analysis settings to a file specified by name.
 */

class TGo4ComSaveAnalysisStatus : public TGo4AnalysisCommand {
  public:

    TGo4ComSaveAnalysisStatus();

    TGo4ComSaveAnalysisStatus(const char *filename);

    Int_t ExeCom() override;

    /** Set internals of this command from specification of external
     * remote command (command container). Must be overloaded
     * for each specific command! */
    void Set(TGo4RemoteCommand *remcon) override;

    /** Access to the object name under command. */
    const char *GetFileName() const { return fxFileName.Data(); }

    /** Set the object name under command. */
    void SetFileName(const char *name) { fxFileName = name; }

  private:

    /** Name of the settings file. */
    TString fxFileName;

  ClassDefOverride(TGo4ComSaveAnalysisStatus,1)
};

#endif //TGO4COMSAVEANALYSISSTATUS_H
