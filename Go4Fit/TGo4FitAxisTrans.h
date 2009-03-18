#ifndef TGO4FITAXISTRANS_H
#define TGO4FITAXISTRANS_H

#include "TGo4FitParsList.h"

/**
 * Base class for axis transformation objects
 */

class TGo4FitAxisTrans : public TGo4FitParsList {
   public:
      TGo4FitAxisTrans();
      TGo4FitAxisTrans(const char* iName, const char* iTitle);
      virtual ~TGo4FitAxisTrans();

      virtual void Transformation(Double_t* scales, Int_t naxis) = 0;

   ClassDef(TGo4FitAxisTrans,1)
};
#endif // TGO4FITAXISTRANS_H
