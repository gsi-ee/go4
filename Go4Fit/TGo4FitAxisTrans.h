// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

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
