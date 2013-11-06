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

#ifndef TGO4COMINITANALYSIS_H
#define TGO4COMINITANALYSIS_H

#include "TGo4AnalysisCommand.h"

/**
 * Executes the InitEventClasses Method of TGo4Analysis framework.
 */
class TGo4ComInitAnalysis : public TGo4AnalysisCommand {
   public:

      TGo4ComInitAnalysis();

      virtual ~TGo4ComInitAnalysis();

      virtual Int_t ExeCom();

   ClassDef(TGo4ComInitAnalysis,1)
};

#endif //TGO4COMINITANALYSIS_H
