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

#ifndef TGO4SOCKETSIGNALHANDLER_H
#define TGO4SOCKETSIGNALHANDLER_H

#include "Rtypes.h"

class TGo4SocketSignalHandler {
   public:

      TGo4SocketSignalHandler(Int_t signum, Bool_t enabled=kTRUE);

      virtual ~TGo4SocketSignalHandler();

      static void SetSigWINCH(Bool_t enabled = kTRUE);

      static bool IsLastSignalWINCH();

      static void DisableSigPipe();

      static void SetSignalAction(Int_t signum, Bool_t enabled=kTRUE);

      static void Handle(int);

      static Int_t GetLastSignal();

      static void SetLastSignal(Int_t v = 0);

   private:
      TGo4SocketSignalHandler();

      static Int_t fgiLastSignal; //! contains last signal handler result
};

#endif //TGO4SOCKETSIGNALHANDLER_H
