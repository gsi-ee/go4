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

#ifndef TGO4MBSRANDOM_H
#define TGO4MBSRANDOM_H

#include "TGo4MbsSource.h"

#define NUM_PEAK 5

class TGo4MbsRandomParameter;

/**
 * Implements a random generator as source for mbs events.
 * Produces events with 2 subevents containing gaussian spectra
 * @author J. Adamczewski, N.Kurz
 * @since 5/2003
 */
class TGo4MbsRandom : public TGo4MbsSource {
  public:

    TGo4MbsRandom(const char* name);

    TGo4MbsRandom();

    TGo4MbsRandom(TGo4MbsRandomParameter* par);

    virtual ~TGo4MbsRandom();

    /** Close the file or connection. */
    virtual Int_t Close();

    /** Open the file or connection. */
    virtual Int_t Open();

    /** Requests the next event from mbs source. Returns
      * event status value. */
    virtual Int_t NextEvent();

    /** User access to current event buffer header. dummy here! */
    virtual s_bufhe * GetBufferHeader();

  private:

    /** total length of current random event in words (Short_t) */
    Int_t fiDLen;

    /** Number of subevents per event */
    Int_t fiNumSub;

    /** Number of data longwords per subevent */
    Int_t fiNumDat;

    /** Memory allocated for the current random event */
    Short_t* fxEventMem; //!

    UInt_t fuSeed;

    /** obtains seed for generator */
    void get_rand_seed();

    double gauss_rnd(double mean, double sigma);

    double get_int(double low, double high);

    long rand_event(long choice);

    static double fgdPeak[];
    static double fgdSigma[];
    static double fgdPeak2[];
    static double fgdSigma2[];

  ClassDef(TGo4MbsRandom,1)
};

#endif //TGO4MBSRANDOM_H
