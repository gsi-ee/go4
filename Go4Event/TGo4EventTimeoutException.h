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

#ifndef TGO4EVENTTIMEOUTEXCEPTION_H
#define TGO4EVENTTIMEOUTEXCEPTION_H

#include "TGo4EventSourceException.h"

  /**
    *   Exception to throw on event source timeout.
    *   (e.g. blocked mbs socket)
    */
class TGo4EventTimeoutException : public TGo4EventSourceException {

  public:

    TGo4EventTimeoutException (TGo4EventSource* eventsource);

    virtual ~TGo4EventTimeoutException();

    TGo4EventTimeoutException(const TGo4EventTimeoutException &right);

    TGo4EventTimeoutException & operator = (const TGo4EventTimeoutException & right);

  private:

    TGo4EventTimeoutException();
};

#endif //TGO4EVENTTIMEOUTEXCEPTION_H
