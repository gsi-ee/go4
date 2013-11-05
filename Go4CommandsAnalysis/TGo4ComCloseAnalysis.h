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

#ifndef TGO4COMCLOSEANALYSIS_H
#define TGO4COMCLOSEANALYSIS_H

#include "TGo4AnalysisCommand.h"

/** Executes the CloseAnalysis Method of TGo4Analysis framework. */

class TGo4ComCloseAnalysis : public TGo4AnalysisCommand {
   public:

      TGo4ComCloseAnalysis();

      virtual ~TGo4ComCloseAnalysis();

      virtual Int_t ExeCom();

   ClassDef(TGo4ComCloseAnalysis,1)
};

#endif //TGO4COMCLOSEANALYSIS_H
