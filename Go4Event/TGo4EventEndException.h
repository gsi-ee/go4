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
