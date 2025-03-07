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

#ifndef TGO4OBJCONNECTORRUNNABLE_H
#define TGO4OBJCONNECTORRUNNABLE_H

#include "TGo4Runnable.h"

class TGo4HistogramServer;

/**
 * Runnable that serves an object request for object client
 */
class TGo4ObjConnectorRunnable : public TGo4Runnable {
public:
   virtual ~TGo4ObjConnectorRunnable();

   TGo4ObjConnectorRunnable();

   TGo4ObjConnectorRunnable(const char *name, TGo4HistogramServer *hserv);

   Int_t Run(void *) override;

private:
   /** @supplierCardinality 1
    * @clientCardinality 1*/
   TGo4HistogramServer *fxHistogramServer{nullptr}; //!
};

#endif //TGO4OBJCONNECTORRUNNABLE_H
