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

#ifndef TGO4FILESOURCEPARAMETER_H
#define TGO4FILESOURCEPARAMETER_H

#include "TGo4EventSourceParameter.h"

class TGo4FileSourceParameter : public TGo4EventSourceParameter {

   public:

      TGo4FileSourceParameter();

      TGo4FileSourceParameter(const char* name);

      virtual ~TGo4FileSourceParameter();

   ClassDef(TGo4FileSourceParameter,1)
};

#endif //TGO4FILESOURCEPARAMETER_H
