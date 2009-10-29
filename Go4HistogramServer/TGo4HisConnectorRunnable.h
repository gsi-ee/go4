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

#ifndef TGO4HISCONNECTORRUNNABLE_H
#define TGO4HISCONNECTORRUNNABLE_H

#include "TGo4Runnable.h"

class TGo4HistogramServer;

/** Runnable that waits for client connection request and
  * starts new HisCliRunnable. */
class TGo4HisConnectorRunnable : public TGo4Runnable {
   public:

      virtual ~TGo4HisConnectorRunnable();

      TGo4HisConnectorRunnable();

      TGo4HisConnectorRunnable(const char* name, TGo4HistogramServer* hserv);

      virtual Int_t Run(void*);

   private:

      /** @supplierCardinality 1
       * @clientCardinality 1*/
      TGo4HistogramServer * fxHistogramServer;  //!
};

#endif //TGO4HISCONNECTORRUNNABLE_H
