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

#ifndef TGO4DISPLAYLOGGERTIMER_H
#define TGO4DISPLAYLOGGERTIMER_H

#include "TTimer.h"

class TGo4Display;

/**
 * timer which belongs to the Display, used instead of a thread to
 * avoid conflicts with the Qt GUI internal slots. This Timer acts
 * in between the status queue and the GUI, e.g. by drawing eventrate
 * information on a Qt widget.
 */

class TGo4DisplayLoggerTimer : public TTimer {

  public:

      TGo4DisplayLoggerTimer (TGo4Display* display, Long_t msperiod = 3000);

      virtual ~TGo4DisplayLoggerTimer();

      virtual Bool_t Notify ();

  private:
      TGo4Display* fxDisplay;    //!

  private:
      TGo4DisplayLoggerTimer();

};

#endif //TGO4DISPLAYLOGGERTIMER_H
