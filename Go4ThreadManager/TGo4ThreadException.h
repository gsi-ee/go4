#ifndef TGo4ThreadException_h
#define TGo4ThreadException_h

#include "TGo4ControlException.h"
#include "TString.h"

class TGo4Runnable;
class TGo4ThreadHandler;

/**
 * Exception base class for all exceptions acting on a thread.
 * @author J. Adamczewski
 * @author Go4 project-DVEE-GSI Darmstadt
 * @author Germany
 * @author j.adamczewski@gsi.de
 * @version 0.9
 * @since jul 2000-oct 2001*/
class TGo4ThreadException : public TGo4ControlException {
   public:
      TGo4ThreadException(const TGo4ThreadException &right);

      TGo4ThreadException (TGo4Runnable* runnable, const Text_t* threadname = 0);

      virtual ~TGo4ThreadException();

      TGo4ThreadException & operator=(const TGo4ThreadException &right);

      /** This is a default handler function for the respective exception type. */
      virtual Int_t Handle () = 0;

      const Text_t* GetThreadName ();

   protected:
      /** Name of the TGo4Thread  that shall be acted on */
      TString fxThreadName;

      /** Link to the runnable connected with the thread
        *   (exception throwing runnable might simply pass its 'this' pointer as argument) **/
      TGo4Runnable *fxRunnable; //!

      /** Link to threadhandler associated with the runnable **/
      TGo4ThreadHandler *fxThreadHandler; //!

   private:
      TGo4ThreadException();
};

#endif
