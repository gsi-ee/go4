#ifndef TGo4CreateException_h
#define TGo4CreateException_h

#include "TGo4ThreadException.h"
/**
 * Create a thread associated with the runnable.
 * @author J. Adamczewski
 * @author Go4 project-DVEE-GSI Darmstadt
 * @author Germany
 * @author j.adamczewski@gsi.de
 * @version 0.9
 * @since jul 2000-oct 2001*/
class TGo4CreateException : public TGo4ThreadException {

  public:
      TGo4CreateException(const TGo4CreateException &right);

      TGo4CreateException (TGo4Runnable* runnable, const char* threadname = 0);

      virtual ~TGo4CreateException();

      TGo4CreateException & operator=(const TGo4CreateException &right);

      /**
       * This is a default handler function for the respective exception type.
       */
      virtual Int_t Handle ();


  private:
      TGo4CreateException();

};


#endif



