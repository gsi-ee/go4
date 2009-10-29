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

#ifndef TGO4MBSEVENTSERVER_H
#define TGO4MBSEVENTSERVER_H

#include "TGo4MbsSource.h"

class TGo4MbsEventServerParameter;

/**
 * Implements the gsi mbs event server into the
 * go4 classes. This is an alias for the MbsSource
 * Class with parameter GETEVT__EVENT
 * @author J. Adamczewski
 * @since 1/2001
 */

class TGo4MbsEventServer : public TGo4MbsSource {
  public:

    TGo4MbsEventServer(const char* name);

    TGo4MbsEventServer();

    TGo4MbsEventServer(TGo4MbsEventServerParameter* par);

    ~TGo4MbsEventServer();

  ClassDef(TGo4MbsEventServer,1)
};

#endif //TGO4MBSEVENTSERVER_H
