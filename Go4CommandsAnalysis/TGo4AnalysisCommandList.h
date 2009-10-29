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

#ifndef TGO4ANALYSISCOMMANDLIST_H
#define TGO4ANALYSISCOMMANDLIST_H

#include "TGo4TaskHandlerCommandList.h"

/** list containing prototypes of all analysis command objects. */

class TGo4AnalysisCommandList : public TGo4TaskHandlerCommandList {

   public:

      TGo4AnalysisCommandList();

      virtual ~TGo4AnalysisCommandList();
};

#endif //TGO4ANALYSISCOMMANDLIST_H
