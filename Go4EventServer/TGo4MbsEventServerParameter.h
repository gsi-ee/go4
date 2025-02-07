// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4MBSEVENTSERVERPARAMETER_H
#define TGO4MBSEVENTSERVERPARAMETER_H

#include "TGo4MbsSourceParameter.h"

/** @brief MBS event server parameter
 * @ingroup go4_event
 */

class TGo4MbsEventServerParameter : public TGo4MbsSourceParameter {
  friend class TGo4MbsEventServer;
  public:
    TGo4MbsEventServerParameter();

    TGo4MbsEventServerParameter(const char *name);

    virtual ~TGo4MbsEventServerParameter();

  ClassDefOverride(TGo4MbsEventServerParameter, 2)
};

#endif //TGO4MBSEVENTSERVERPARAMETER_H
