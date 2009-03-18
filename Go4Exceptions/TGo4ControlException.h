#ifndef TGo4ControlException_H
#define TGo4ControlException_H

#include "TGo4RuntimeException.h"

/** Exception Class responsible for runtime control actions:
  * Exception mechanism is used as command pattern here;
  * concrete exceptions act on single threads or the thread manager */

class TGo4ControlException : public TGo4RuntimeException {
   public:
      TGo4ControlException();

      TGo4ControlException(const TGo4ControlException &right);

      virtual ~TGo4ControlException();

      TGo4ControlException & operator=(const TGo4ControlException &right);

      /** This is a default handler function for the respective exception type */
      virtual Int_t Handle();
};

#endif
