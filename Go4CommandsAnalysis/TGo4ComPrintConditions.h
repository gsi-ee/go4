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

#ifndef TGO4COMPRINTCONDITIONS_H
#define TGO4COMPRINTCONDITIONS_H

#include "TGo4AnalysisCommand.h"

/**
 * Printout all condition counters as bar diagram on the local analysis terminal.
 * May send this information to the gui later if option local is kFALSE.
 */
class TGo4ComPrintConditions : public TGo4AnalysisCommand {
  public:

    TGo4ComPrintConditions();

    virtual ~TGo4ComPrintConditions();

    Int_t ExeCom();

  ClassDef(TGo4ComPrintConditions,1)
};

#endif //TGO4COMPRINTCONDITIONS_H
