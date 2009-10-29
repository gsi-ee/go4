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

#ifndef TGO4MBSSTREAMPARAMETER_H
#define TGO4MBSSTREAMPARAMETER_H

#include "TGo4MbsSourceParameter.h"

class TGo4MbsStreamParameter : public TGo4MbsSourceParameter {
  public:
    TGo4MbsStreamParameter() ;

    TGo4MbsStreamParameter(const char* name);

    virtual ~TGo4MbsStreamParameter() ;

  ClassDef(TGo4MbsStreamParameter, 2)
};

#endif //TGO4MBSSTREAMPARAMETER_H
