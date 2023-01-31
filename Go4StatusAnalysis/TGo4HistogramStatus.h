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

#ifndef TGO4HISTOGRAMSTATUS_H
#define TGO4HISTOGRAMSTATUS_H

#include "TGo4ObjectStatus.h"

class TH1;

/**
 * Runtime status of a histogram object.
 * @author J. Adamczewski
 * @since 09-jul-2001
 */
class TGo4HistogramStatus : public TGo4ObjectStatus {

  friend class TGo4AnalysisProxy;

  public:

    TGo4HistogramStatus();

    /** Create status from given histogram object.
     *  If argument allstatistics is kTRUE, system time and mean/rms
     *  is calculated as well. Default is disabled, since
     *  2d histograms are very time consuming here! */
    TGo4HistogramStatus(TH1 *his, Bool_t allstatistics=kFALSE);

    virtual ~TGo4HistogramStatus();

    void Print(Option_t* = "") const override;

    Int_t GetDimension() const { return fiDim; }

    Int_t GetXbins() const { return fiNbinX; }

    Int_t GetYbins() const { return fiNbinY; }

    Int_t GetZbins() const { return fiNbinZ; }

    Axis_t GetXmin() const { return fdXmin; }

    Axis_t GetXmax() const { return fdXmax; }

    Axis_t GetYmin() const { return fdYmin; }

    Axis_t GetYmax() const { return fdYmax;}

    Axis_t GetZmin() const { return fdZmin;}

    Axis_t GetZmax() const { return fdZmax;}

    Stat_t GetEntries() const { return fdEntries;}

    Stat_t GetXmean() const { return fdXmean; }

    Stat_t GetYmean() const { return fdYmean; }

    Stat_t GetZmean() const { return fdZmean; }

    Stat_t GetXrms() const { return fdXrms; }

    Stat_t GetYrms() const { return fdYrms; }

    Stat_t GetZrms() const { return fdZrms; }

  private:

    /** Histogram dimension (1,2,3). */
    Int_t fiDim{0};

    /** Number of bins in x axis. */
    Int_t fiNbinX{0};

    /** Number of bins in y axis. */
    Int_t fiNbinY{0};

    /** Number of bins in z axis. */
    Int_t fiNbinZ{0};

    /** Lower edge of x axis. */
    Axis_t fdXmin{0};

    /** Upper edge of x axis. */
    Axis_t fdXmax{0};

    /** Lower edge of y axis. */
    Axis_t fdYmin{0};

    /** Upper edge of y axis. */
    Axis_t fdYmax{0};

    /** Lower edge of z axis. */
    Axis_t fdZmin{0};

    /** Upper edge of z axis. */
    Axis_t fdZmax{0};

    /** Number of filled entries. */
    Stat_t fdEntries{0};

    /** Mean value in x projection. */
    Stat_t fdXmean{0};

    /** Mean value in y projection. */
    Stat_t fdYmean{0};

    /** Mean value in z projection. */
    Stat_t fdZmean{0};

    /** Root mean square value in x projection. */
    Stat_t fdXrms{0};

    /** Root mean square value in y projection. */
    Stat_t fdYrms{0};

    /** Root mean square value in z projection. */
    Stat_t fdZrms{0};

  ClassDefOverride(TGo4HistogramStatus, 1)
};

#endif //TGO4HISTOGRAMSTATUS_H
