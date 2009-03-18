#ifndef TGO4DISPLAYDRAWERTIMER_H
#define TGO4DISPLAYDRAWERTIMER_H

#include "TTimer.h"

class TGo4Display;

/**
 * timer which belongs to the Display, used instead of a thread to
 * avoid conflicts with the Qt GUI internal slots. This Timer acts
 * in between the data queue and the GUI, e.g. by drawing histograms
 * on a viewpanel or putting them into the GUI lists.
 */

class TGo4DisplayDrawerTimer : public TTimer {

  public:

      TGo4DisplayDrawerTimer (TGo4Display* display, Long_t msperiod = 3000);

      virtual ~TGo4DisplayDrawerTimer();

      virtual Bool_t Notify ();

    private:

      TGo4Display* fxDisplay;   //!

      TGo4DisplayDrawerTimer();
};

#endif //TGO4DISPLAYDRAWERTIMER_H




