// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4VERSION_H
#define TGO4VERSION_H

#include "Rtypes.h"

// This central definition will set the current Go4 Build Version
// will be used by TGo4Analysis (dynamic) and TGo4Version (static) singletons
// to check the user analysis against framework version on runtime
// 10205 means Version 1.02.05 or release number  1.2-5,

#define __GO4BUILDVERSION__ 50300
#define __GO4RELEASE__ "v5.3.0"

class TGo4Version {
   protected:
      TGo4Version();

   public:

      virtual ~TGo4Version();

      static TGo4Version * Instance();

      /** Returns true if Version number matches the
        * argument of the Method. */
      static Bool_t CheckVersion(Int_t version);

      /** Version number at build time of user analysis. */
      static Int_t GetBuildVersion();

   private:
      /** Version Number of the Go4 build. Number 10000 means Version 1.00.00.
        * Will be compared to the Version number in static Version singleton on
        * runtime. */
      static const Int_t fgiGO4VERSION;

      static TGo4Version * fxInstance;

};

#endif //TGO4VERSION_H
