#ifndef TGO4USEREXCEPTION_H
#define TGO4USEREXCEPTION_H

#include "TGo4RuntimeException.h"

#include "TString.h"

/**
 * Exception to be thrown by analysis user.
 * @author J. Adamczewski
 * @since Sep-2002
 */

class TGo4UserException : public TGo4RuntimeException {

   public:

      TGo4UserException (Int_t prio, const char* text,...);

      virtual ~TGo4UserException();

      Int_t Handle ();

      TGo4UserException(const TGo4UserException &right);

      TGo4UserException & operator = (const TGo4UserException & right);

      /** Message priority */
      Int_t GetPriority() const { return fiPriority; }

      /** Status message of last Process() call. */
      const char* GetMessage() const { return fxMessage.Data(); }

   protected:

      /** User defined description of the exception reason */
      TString fxMessage;

      /** Priority of message. 1=info, 2= warning, 3=error.*/
      Int_t fiPriority;

   private:
      TGo4UserException();
};

#endif // #define
