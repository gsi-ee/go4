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
