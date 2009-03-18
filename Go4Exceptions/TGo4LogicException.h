#ifndef TGo4LogicException_H
#define TGo4LogicException_H

#include "TGo4Exception.h"

class TGo4LogicException : public TGo4Exception {
   public:
      TGo4LogicException();

      TGo4LogicException(const TGo4LogicException &right);

      virtual ~TGo4LogicException();

      TGo4LogicException & operator=(const TGo4LogicException &right);

      /** This is a default handler function for the respective exception type **/
      virtual Int_t Handle();
};

#endif
