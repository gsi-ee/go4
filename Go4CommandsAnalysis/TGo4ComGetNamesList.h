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

#ifndef TGO4COMGETNAMESLIST_H
#define TGO4COMGETNAMESLIST_H

#include "TGo4AnalysisCommand.h"

/**
 * Requests for the current folder list of analysis
 * objects.
 */

class TGo4ComGetNamesList : public TGo4AnalysisCommand {
  public:

    TGo4ComGetNamesList();

    virtual ~TGo4ComGetNamesList();

    Int_t ExeCom();

  ClassDef(TGo4ComGetNamesList,1)
};

#endif //TGO4COMGETNAMESLIST_H
