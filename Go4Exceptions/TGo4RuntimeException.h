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



