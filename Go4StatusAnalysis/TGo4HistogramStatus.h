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

#ifndef TGO4HISTOGRAMSTATUS_H
#define TGO4HISTOGRAMSTATUS_H

#include "TGo4ObjectStatus.h"

#include "Htypes.h"

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
    TGo4HistogramStatus(TH1* his, Bool_t allstatistics=kFALSE) ;

    virtual ~TGo4HistogramStatus() ;



    /** basic method to printout status information
      * on stdout; to be overridden by specific subclass */
    virtual Int_t PrintStatus(Text_t* buffer=0, Int_t buflen=0);

    Int_t GetDimension(){ return fiDim; }

    Int_t GetXbins(){ return fiNbinX; }

    Int_t GetYbins(){ return fiNbinY; }

    Int_t GetZbins(){ return fiNbinZ; }

    Axis_t GetXmin(){ return fdXmin; }

    Axis_t GetXmax(){ return fdXmax; }

    Axis_t GetYmin(){ return fdYmin; }

    Axis_t GetYmax(){ return fdYmax;}

    Axis_t GetZmin(){ return fdZmin;}

    Axis_t GetZmax(){ return fdZmax;}

    Stat_t GetEntries(){ return fdEntries;}

    Stat_t GetXmean(){ return fdXmean; }

    Stat_t GetYmean(){ return fdYmean; }

    Stat_t GetZmean(){ return fdZmean; }

    Stat_t GetXrms(){ return fdXrms; }

    Stat_t GetYrms(){ return fdYrms; }

    Stat_t GetZrms(){ return fdZrms; }

  private:

    /** Histogram dimension (1,2,3). */
    Int_t fiDim;

    /** Number of bins in x axis. */
    Int_t fiNbinX;

    /** Number of bins in y axis. */
    Int_t fiNbinY;

    /** Number of bins in z axis. */
    Int_t fiNbinZ;

    /** Lower edge of x axis. */
    Axis_t fdXmin;

    /** Upper edge of x axis. */
    Axis_t fdXmax;

    /** Lower edge of y axis. */
    Axis_t fdYmin;

    /** Upper edge of y axis. */
    Axis_t fdYmax;

    /** Lower edge of z axis. */
    Axis_t fdZmin;

    /** Upper edge of z axis. */
    Axis_t fdZmax;

    /** Number of filled entries. */
    Stat_t fdEntries;

    /** Mean value in x projection. */
    Stat_t fdXmean;

    /** Mean value in y projection. */
    Stat_t fdYmean;

    /** Mean value in z projection. */
    Stat_t fdZmean;

    /** Root mean square value in x projection. */
    Stat_t fdXrms;

    /** Root mean square value in y projection. */
    Stat_t fdYrms;

    /** Root mean square value in z projection. */
    Stat_t fdZrms;

  ClassDef(TGo4HistogramStatus,1 )
};

#endif //TGO4HISTOGRAMSTATUS_H
