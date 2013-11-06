// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4COMPRINTHISTOGRAMS_H
#define TGO4COMPRINTHISTOGRAMS_H

#include "TGo4AnalysisCommand.h"

/**
 * Printout all histogram statisticx on the local analysis terminal.
 * May send this information to the gui later if option fbLocal is set to kFALSE.
 */
class TGo4ComPrintHistograms : public TGo4AnalysisCommand {
   public:

      TGo4ComPrintHistograms();

      virtual ~TGo4ComPrintHistograms();

      virtual Int_t ExeCom();

   ClassDef(TGo4ComPrintHistograms,1)
};
#endif //TGO4COMPRINTHISTOGRAMS_H
