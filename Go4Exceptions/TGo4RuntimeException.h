#ifndef TGo4RuntimeException_H
#define TGo4RuntimeException_H

#include "TGo4Exception.h"

class TGo4RuntimeException : public TGo4Exception {
   public:
      TGo4RuntimeException();

      TGo4RuntimeException(const TGo4RuntimeException &right);

      virtual ~TGo4RuntimeException();

      TGo4RuntimeException & operator=(const TGo4RuntimeException &right);

      /** This is a default handler function for the respective exception type **/
      virtual Int_t Handle();
};

#endif



