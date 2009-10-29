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

#ifndef TGO4MBSSTREAM_H
#define TGO4MBSSTREAM_H

#include "TGo4MbsSource.h"

class TGo4MbsStreamParameter;

/**
 * Implements the gsi mbs stream server into the
 * go4 classes. This is an alias for the MbsSource
 * Class with parameter GETEVT__STREAM
 * @author J. Adamczewski
 * @since 1/2001
 */
class TGo4MbsStream : public TGo4MbsSource {
  public:

    TGo4MbsStream(const char* name);

    TGo4MbsStream();

    TGo4MbsStream(TGo4MbsStreamParameter* par);

    ~TGo4MbsStream();

  ClassDef(TGo4MbsStream,1)
};

#endif //TGO4MBSSTREAM_H
