#ifndef TGo4Exception_H
#define TGo4Exception_H

#include "TString.h"
#include "TObject.h"

class TGo4Exception : public TObject {
  public:
      TGo4Exception();

      TGo4Exception(const TGo4Exception &right);

      virtual ~TGo4Exception();

      TGo4Exception & operator=(const TGo4Exception &right);

      /** Returns string describing the kind of exception **/
      virtual const char* What();

      /** This is a default handler function for the respective exception type **/
      virtual Int_t Handle();

  protected:

      TString fxDescription;
};

#endif
