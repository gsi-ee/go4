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

#ifndef TGO4COMPRINTDYNLIST_H
#define TGO4COMPRINTDYNLIST_H

#include "TGo4AnalysisCommand.h"

/**
 * Printout of dynamic list status on the local analysis terminal.
 * May send this information to the gui later if option fbLocal is set to kFALSE.
 */
class TGo4ComPrintDynList : public TGo4AnalysisCommand {
public:

    TGo4ComPrintDynList();

    virtual ~TGo4ComPrintDynList();

    Int_t ExeCom();

  ClassDef(TGo4ComPrintDynList,1)
};

#endif //TGO4COMPRINTDYNLIST_H
