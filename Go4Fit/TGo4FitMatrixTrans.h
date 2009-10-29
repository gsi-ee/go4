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

#ifndef TGO4FITMATRIXTRANS_H
#define TGO4FITMATRIXTRANS_H

#include "TGo4FitAxisTrans.h"

/**
 * Matrix transfromation of scale values.
 *
 * It has full matrix (N x N) to transfrom axis values like:
 *
 *   scale' = Matrix x scale
 *
 * Each matrix element can be changed indvidually
 */
class TGo4FitMatrixTrans : public TGo4FitAxisTrans {
   public:
      TGo4FitMatrixTrans();
      TGo4FitMatrixTrans(const char* iName, const char* iTitle, Int_t NumAxis = 1);
      virtual ~TGo4FitMatrixTrans();

      Int_t GetNumAxis();
      void SetNumAxis(Int_t NumAxis);

      virtual void Transformation(Double_t* scales, Int_t naxis);

      virtual void Print(Option_t* option) const;

   protected:

   ClassDef(TGo4FitMatrixTrans,1)
};
#endif // TGO4FITMATRIXTRANS_H
