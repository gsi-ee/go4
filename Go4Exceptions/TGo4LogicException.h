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
