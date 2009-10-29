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

#ifndef TGO4TREESOURCEPARAMETER_H
#define TGO4TREESOURCEPARAMETER_H

#include "TGo4EventSourceParameter.h"

class TGo4TreeSourceParameter : public TGo4EventSourceParameter {
  public:
     TGo4TreeSourceParameter();

     TGo4TreeSourceParameter(const char* name);

     virtual ~TGo4TreeSourceParameter() ;

  ClassDef(TGo4TreeSourceParameter,1)
};

#endif //TGO4TREESOURCEPARAMETER_H
