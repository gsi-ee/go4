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
