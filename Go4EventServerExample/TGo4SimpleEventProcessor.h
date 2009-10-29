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

#ifndef TGO4SIMPLEEVENTPROCESSOR_H
#define TGO4SIMPLEEVENTPROCESSOR_H

#include "TGo4EventProcessor.h"

class TGo4SimpleEvent;

class TGo4SimpleEventProcessor : public TGo4EventProcessor {
   public:

      TGo4SimpleEventProcessor();

      void BuildSimpleEvent(TGo4SimpleEvent* target);

      virtual ~TGo4SimpleEventProcessor() ;

  ClassDef(TGo4SimpleEventProcessor,1)
};

#endif //TGO4SIMPLEEVENTPROCESSOR_H
