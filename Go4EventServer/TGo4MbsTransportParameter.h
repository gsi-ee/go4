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

#ifndef TGO4MBSTRANSPORTPARAMETER_H
#define TGO4MBSTRANSPORTPARAMETER_H

#include "TGo4MbsSourceParameter.h"

class TGo4MbsTransportParameter : public TGo4MbsSourceParameter {
  public:
    TGo4MbsTransportParameter() ;

    TGo4MbsTransportParameter(const char* name);

    virtual ~TGo4MbsTransportParameter() ;

  ClassDef(TGo4MbsTransportParameter, 2)
};

#endif //TGO4MBSTRANSPORTPARAMETER_H
