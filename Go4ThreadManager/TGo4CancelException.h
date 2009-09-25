#ifndef TGo4CancelException_h
#define TGo4CancelException_h

#include "TGo4ThreadException.h"

/**
 * Cancel thread associated with runnable or with threadname.
 * @author J. Adamczewski
 * @author Go4 project-DVEE-GSI Darmstadt
 * @author Germany
 * @author j.adamczewski@gsi.de
 * @version 0.9
 * @since jul 2000-oct 2001*/
class TGo4CancelException : public TGo4ThreadException
{
  public:
      TGo4CancelException(const TGo4CancelException &right);

      TGo4CancelException (TGo4Runnable* runnable, const char* threadname = 0);

      virtual ~TGo4CancelException();

      TGo4CancelException & operator=(const TGo4CancelException &right);

     /**
       * This is a default handler function for the respective exception type.
       */
      virtual Int_t Handle ();


  private:
      TGo4CancelException();

};


#endif



