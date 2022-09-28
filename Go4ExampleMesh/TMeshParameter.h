// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef MESHPAR_H
#define MESHPAR_H

#include "TGo4Parameter.h"

/** This parameter may be used for a global setup of the mesh */

class TMeshParameter : public TGo4Parameter {
   public:
      TMeshParameter();
      TMeshParameter(const char *name);
      virtual ~TMeshParameter();
      void Print(Option_t* = "") const override;
      Bool_t UpdateFrom(TGo4Parameter *) override;

      Bool_t fbUnpackOn{kTRUE}; // switch on unpack step
      Bool_t fbExec1On{kTRUE};  // switch on branch 1 step
      Bool_t fbExec2On{kTRUE};  // switch on branch 2 step
      Bool_t fbExec3On{kTRUE};  // switch on branch 3 step
      Bool_t fbExec12On{kTRUE}; // switch on branch 1+2 combination step
      Bool_t fbCollectorOn{kTRUE}; // switch on final step

   ClassDefOverride(TMeshParameter,1)
};

#endif //MESHPAR_H





