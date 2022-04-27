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

#ifndef TGO4OBJCONNECTORTIMER_H
#define TGO4OBJCONNECTORTIMER_H

#include "TTimer.h"

class TGo4HistogramServer;

/**
 * timer aggregated to the histogram server which is responsible to
 * establish connections of the go4 object clients.
 */

class TGo4ObjConnectorTimer : public TTimer {

public:
   TGo4ObjConnectorTimer(TGo4HistogramServer *server, Long_t msperiod = 3000);

   virtual ~TGo4ObjConnectorTimer();

   Bool_t Notify() override;

protected:
   /** the object server to which this timer belongs */
   TGo4HistogramServer *fxServer{nullptr}; //!
};

#endif //TGO4TASKCONNECTORTIMER_H
