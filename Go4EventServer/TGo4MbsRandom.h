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

#ifndef TGO4MBSRANDOM_H
#define TGO4MBSRANDOM_H

#include "TGo4MbsSource.h"

#define NUM_PEAK 5

class TGo4MbsRandomParameter;

/**
 * @brief Implements a random generator as source for mbs events.
 * @details Produces events with 2 subevents containing gaussian spectra
 * @ingroup go4_event
 * @author J. Adamczewski, N.Kurz
 * @since 5/2003
 */
class TGo4MbsRandom : public TGo4MbsSource {
  public:

    TGo4MbsRandom(const char *name);

    TGo4MbsRandom();

    TGo4MbsRandom(TGo4MbsRandomParameter *par);

    virtual ~TGo4MbsRandom();

    /** Close the file or connection. */
    Int_t Close() override;

    /** Open the file or connection. */
    Int_t Open() override;

    /** Requests the next event from mbs source. Returns
      * event status value. */
    Int_t NextEvent() override;

    /** User access to current event buffer header. dummy here! */
    s_bufhe * GetBufferHeader() override;

  private:

    /** total length of current random event in words (Short_t) */
    Int_t fiDLen{0};

    /** Number of subevents per event */
    Int_t fiNumSub{0};

    /** Number of data longwords per subevent */
    Int_t fiNumDat{0};

    /** Memory allocated for the current random event */
    Short_t *fxEventMem{nullptr}; //!

    UInt_t fuSeed{0};

    /** obtains seed for generator */
    void get_rand_seed();

    double gauss_rnd(double mean, double sigma);

    double get_int(double low, double high);

    long rand_event(long choice);

  ClassDefOverride(TGo4MbsRandom,1)
};

#endif //TGO4MBSRANDOM_H
