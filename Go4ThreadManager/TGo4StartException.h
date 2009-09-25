#ifndef TGo4StartException_h
#define TGo4StartException_h

#include "TGo4ThreadException.h"

/**
 * start thread associated with runnable.
 * @author J. Adamczewski
 * @author Go4 project-DVEE-GSI Darmstadt
 * @author Germany
 * @author j.adamczewski@gsi.de
 * @version 0.9
 * @since jul 2000-oct 2001*/
class TGo4StartException : public TGo4ThreadException {

  public:
      TGo4StartException(const TGo4StartException &right);

      TGo4StartException (TGo4Runnable* runnable, const char* threadname = 0);

      virtual ~TGo4StartException();

      TGo4StartException & operator=(const TGo4StartException &right);

      virtual Int_t Handle ();

  private:
      TGo4StartException();
};

#endif
