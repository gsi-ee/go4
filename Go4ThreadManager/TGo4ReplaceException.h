#ifndef TGo4ReplaceException_h
#define TGo4ReplaceException_h

#include "TGo4ThreadException.h"

class TGo4Runnable;

/**
*   Exception with the effect of removing old TGo4Thread from
*   TGo4ThreadHandler and creating and starting new
*   TGo4Thread with new runnable fxNewRunnable.
*   If threadname is not specified in ctor, the thread of the
*   first runnable given (usually the calling runnable) is
*   replaced;
*   if new runnable is not given, the thread of the calling
*   runnable will be removed and a new thread with the
*   calling runnable will be created, added to the handler
*   list and started
* @author J. Adamczewski
* @author Go4 project-DVEE-GSI Darmstadt
* @author Germany
* @author j.adamczewski@gsi.de
* @version 0.9
* @since jul 2000-oct 2001*/

class TGo4ReplaceException : public TGo4ThreadException {
  public:
      TGo4ReplaceException(const TGo4ReplaceException &right);

      TGo4ReplaceException (TGo4Runnable* runnable, TGo4Runnable* newrunnable = 0, const char* oldthreadname = 0);

      virtual ~TGo4ReplaceException();

      TGo4ReplaceException & operator=(const TGo4ReplaceException &right);

      virtual Int_t Handle ();

  protected:
      TGo4Runnable *fxNewRunnable; //!

  private:
      TGo4ReplaceException();

};

#endif
