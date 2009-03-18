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
