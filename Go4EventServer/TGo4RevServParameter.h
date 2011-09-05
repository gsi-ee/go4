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

#ifndef TGO4REVSERVPARAMETER_H
#define TGO4REVSERVPARAMETER_H

#include "TGo4MbsSourceParameter.h"

class TGo4RevServParameter : public TGo4MbsSourceParameter {
  public:
    TGo4RevServParameter();

    TGo4RevServParameter(const char* name, Int_t defport = 0);

    virtual ~TGo4RevServParameter();

  private:

  ClassDef(TGo4RevServParameter, 3)
};

#endif //TGO4REVSERVPARAMETER_H
