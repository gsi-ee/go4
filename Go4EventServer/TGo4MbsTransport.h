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

#ifndef TGO4MBSTRANSPORT_H
#define TGO4MBSTRANSPORT_H

#include "TGo4MbsSource.h"

class TGo4MbsTransportParameter;

/**
 * @brief Implements the gsi mbs transport server into the go4 classes.
 * @details This is an alias for the MbsSource
 * Class with parameter GETEVT__TRANS
 * @ingroup go4_event
 * @author J. Adamczewski
 * @since 1/2001
 */
class TGo4MbsTransport : public TGo4MbsSource {
  public:

    TGo4MbsTransport(const char *name);

    TGo4MbsTransport();

    TGo4MbsTransport(TGo4MbsTransportParameter *par);

    ~TGo4MbsTransport();

  ClassDefOverride(TGo4MbsTransport,1)
};

#endif //TGO4MBSTRANSPORT_H
