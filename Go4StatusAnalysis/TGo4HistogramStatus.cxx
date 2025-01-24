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

#include "TGo4HistogramStatus.h"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

#include "TGo4Log.h"

TGo4HistogramStatus::TGo4HistogramStatus() : TGo4ObjectStatus()
{
}

TGo4HistogramStatus::TGo4HistogramStatus(TH1 *his, Bool_t allstatistics)
   : TGo4ObjectStatus(his, allstatistics)

{
   GO4TRACE((14,"TGo4HistogramStatus::TGo4HistogramStatus(TH1 *)",__LINE__, __FILE__));

   if (his) {
      fiDim = his->GetDimension();
      fiNbinX = his->GetNbinsX();
      fiNbinY = his->GetNbinsY();
      fiNbinZ = his->GetNbinsZ();
      TAxis *xax = his->GetXaxis();
      if (xax) {
         fdXmin = xax->GetXmin();
         fdXmax = xax->GetXmax();
      } else { // values already initialized as zero
      }
      TAxis *yax = his->GetYaxis();
      if (yax) {
         fdYmin = yax->GetXmin();
         fdYmax = yax->GetXmax();
      } else { // values already initialized as zero
      }
      TAxis *zax = his->GetZaxis();
      if (zax) {
         fdZmin = zax->GetXmin();
         fdZmax = zax->GetXmax();
      } else { // values already initialized as zero
      }
      fdEntries = his->GetEntries();
      //////////////////////////////////////////////////////
      ///// correct object size for real size of data array:
      Int_t fieldsize = 4; // use as default bin size 4 bytes
      // because of multiple inheritance of histogram types, we
      // must check for all types separately to downcast the correct TArray:
      if (his->InheritsFrom(TH1D::Class()) || his->InheritsFrom(TH2D::Class()) || his->InheritsFrom(TH3D::Class())) {
         fieldsize = sizeof(Double_t);
      } else if (his->InheritsFrom(TH1F::Class()) || his->InheritsFrom(TH2F::Class()) || his->InheritsFrom(TH3F::Class())) {
         fieldsize = sizeof(Float_t);
      } else if (his->InheritsFrom(TH1I::Class()) || his->InheritsFrom(TH2I::Class()) || his->InheritsFrom(TH3I::Class())) {
         fieldsize = sizeof(Int_t);
      } else if (his->InheritsFrom(TH1S::Class()) || his->InheritsFrom(TH2S::Class()) || his->InheritsFrom(TH3S::Class())) {
         fieldsize = sizeof(Short_t);
      } else if (his->InheritsFrom(TH1C::Class()) || his->InheritsFrom(TH2C::Class()) || his->InheritsFrom(TH3C::Class())) {
         fieldsize = sizeof(Char_t);
      } else if (his->InheritsFrom(TH1L::Class()) || his->InheritsFrom(TH2L::Class()) || his->InheritsFrom(TH3L::Class())) {
         fieldsize = sizeof(Long64_t);
      }
      fiObjectSize += fieldsize * his->GetNcells(); // add heap field to object stack memory size
      //////////////////////////////////////////////////////////////////

      if (allstatistics) {
         // note: we do not calculate statistic for nameslist
         // slows down when processing empty 2d histos!
         fdXmean = his->GetMean(1);
         fdYmean = his->GetMean(2);
         fdZmean = his->GetMean(3);
         fdXrms = his->GetRMS(1);
         fdYrms = his->GetRMS(2);
         fdZrms = his->GetRMS(3);
      }
   }
}


TGo4HistogramStatus::~TGo4HistogramStatus()
{
   GO4TRACE((14,"TGo4HistogramStatus::TGo4HistogramStatus()",__LINE__, __FILE__));
}

void TGo4HistogramStatus::Print(Option_t *) const
{
   TGo4ObjectStatus::Print();
   PrintLine("G-OOOO-> Histogram Status Class Printout <-OOOO-G");
   PrintLine("G-OOOO-> ---------------------------------------------- <-OOOO-G");
   // put printout of histogram infos here:
   PrintLine(" Dimension: \t%d", GetDimension());
   PrintLine(" Bins X: \t%d", GetXbins());
   PrintLine(" X min: \t\t%f", GetXmin());
   PrintLine(" X max: \t\t%f", GetXmax());
   PrintLine(" X mean: \t%f", GetXmean());
   PrintLine(" X rms: \t\t%f", GetXrms());
   if (GetDimension() > 1) {
      PrintLine(" Bins Y: \t%d", GetYbins());
      PrintLine(" Y min: \t\t%f", GetYmin());
      PrintLine(" Y max: \t\t%f", GetYmax());
      PrintLine(" Y mean: \t%f", GetYmean());
      PrintLine(" Y rms: \t\t%f", GetYrms());
   }
   if (GetDimension() > 2) {
      PrintLine(" Bins Z: \t%d", GetZbins());
      PrintLine(" Z min: \t\t%f", GetZmin());
      PrintLine(" Z max: \t\t%f", GetZmax());
      PrintLine(" Z mean: \t%f", GetZmean());
      PrintLine(" Z rms: \t\t%f", GetZrms());
   }
   PrintLine(" N Entries: \t%f", GetEntries());
}
