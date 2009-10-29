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

#ifndef TGO4EVENTENDEXCEPTION_H
#define TGO4EVENTENDEXCEPTION_H

#include "TGo4EventSourceException.h"

/**  Exception to throw on event source timeout.
  *   (e.g. blocked mbs socket) */
class TGo4EventEndException : public TGo4EventSourceException {

  public:

    TGo4EventEndException (TGo4EventSource* eventsource);

    virtual ~TGo4EventEndException();

    TGo4EventEndException(const TGo4EventEndException &right);

    TGo4EventEndException & operator = (const TGo4EventEndException & right);

  private:

    TGo4EventEndException();
};

#endif //TGO4EVENTENDEXCEPTION_H
