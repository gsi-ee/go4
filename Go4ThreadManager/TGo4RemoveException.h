#ifndef TGo4RemoveException_h
#define TGo4RemoveException_h

#include "TGo4ThreadException.h"

/**
*   Exception with default handler which removes TGo4Thread
*   of given name from the threadhandlerlist of given
*   TGo4Runnable (usually the exception throwing runnable)
*   and deletes it; if no threadname is specified,
*   the respective thread of the runnable is removed
* @author J. Adamczewski
* @author Go4 project-DVEE-GSI Darmstadt
* @author Germany
* @author j.adamczewski@gsi.de
* @version 0.9
* @since jul 2000-oct 2001*/
class TGo4RemoveException : public TGo4ThreadException {

  public:
      TGo4RemoveException(const TGo4RemoveException &right);

      TGo4RemoveException (TGo4Runnable* runnable, const Text_t* threadname = 0);

      virtual ~TGo4RemoveException();

      TGo4RemoveException & operator=(const TGo4RemoveException &right);

      virtual Int_t Handle ();


  private:
      TGo4RemoveException();

};

#endif
