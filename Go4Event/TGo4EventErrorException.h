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

#ifndef TGO4EVENTERROREXCEPTION_H
#define TGO4EVENTERROREXCEPTION_H

#include "TGo4EventSourceException.h"

class TGo4EventSource;

   /**
    *   Exception to be thrown on error of event source.
    *   Mbs Error codes/messages are accessible from exception.
    */
class TGo4EventErrorException : public TGo4EventSourceException {

  public:


    TGo4EventErrorException (TGo4EventSource* eventsource, Int_t prio=3);

    virtual ~TGo4EventErrorException();

    TGo4EventErrorException(const TGo4EventErrorException &right);

    TGo4EventErrorException & operator = (const TGo4EventErrorException & right);

    Int_t GetPriority() const { return fiPriority; }

  private:

    TGo4EventErrorException();

    /* priority of error. 0 means print information only */
    Int_t fiPriority;
};

#endif //TGO4EVENTERROREXCEPTION_H
