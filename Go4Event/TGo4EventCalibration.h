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

#ifndef TGO4EVENTCALIBRATION_H
#define TGO4EVENTCALIBRATION_H

#include "TGo4Parameter.h"


/** Data object for calibration of the input event.
 * Is used by event processor to get external parameters
 * during runtime without re-creating the event processor
 * each time.
 * @interface */
class TGo4EventCalibration : public TGo4Parameter {
   public:

      TGo4EventCalibration();

      TGo4EventCalibration(const char* name);

      virtual ~TGo4EventCalibration();

      /** update contents of paramter class with external object. */
      virtual Bool_t UpdateFrom(TGo4Parameter* rhs);


  ClassDef(TGo4EventCalibration,1)
};
#endif //TGO4EVENTCALIBRATION_H
