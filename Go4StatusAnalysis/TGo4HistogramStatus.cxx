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
      Int_t fieldsize = 0;
      // because of multiple inheritance of histogram types, we
      // must check for all types separately to downcast the correct TArray:
      if (his->InheritsFrom(TH1D::Class())) {
         TH1D *dhis = dynamic_cast<TH1D*>(his);
         fieldsize = sizeof(Double_t) * dhis->GetSize(); // method of TArray baseclass
      } else if (his->InheritsFrom(TH1F::Class())) {
         TH1F *fhis = dynamic_cast<TH1F*>(his);
         fieldsize = sizeof(Float_t) * fhis->GetSize(); // method of TArray baseclass
      } else if (his->InheritsFrom(TH1I::Class())) {
         TH1I *ihis = dynamic_cast<TH1I*>(his);
         fieldsize = sizeof(Int_t) * ihis->GetSize(); // method of TArray baseclass
      } else if (his->InheritsFrom(TH1S::Class())) {
         TH1S *shis = dynamic_cast<TH1S*>(his);
         fieldsize = sizeof(Short_t) * shis->GetSize(); // method of TArray baseclass
      } else if (his->InheritsFrom(TH1C::Class())) {
         TH1C *chis = dynamic_cast<TH1C*>(his);
         fieldsize = sizeof(Char_t) * chis->GetSize(); // method of TArray baseclass
      } else if (his->InheritsFrom(TH2D::Class())) {
         TH2D *dhis = dynamic_cast<TH2D*>(his);
         fieldsize = sizeof(Double_t) * dhis->GetSize(); // method of TArray baseclass
      } else if (his->InheritsFrom(TH2F::Class())) {
         TH2F *fhis = dynamic_cast<TH2F*>(his);
         fieldsize = sizeof(Float_t) * fhis->GetSize(); // method of TArray baseclass
      } else if (his->InheritsFrom(TH2I::Class())) {
         TH2I *ihis = dynamic_cast<TH2I*>(his);
         fieldsize = sizeof(Int_t) * ihis->GetSize(); // method of TArray baseclass
      } else if (his->InheritsFrom(TH2S::Class())) {
         TH2S *shis = dynamic_cast<TH2S*>(his);
         fieldsize = sizeof(Short_t) * shis->GetSize(); // method of TArray baseclass
      } else if (his->InheritsFrom(TH2C::Class())) {
         TH2C *chis = dynamic_cast<TH2C*>(his);
         fieldsize = sizeof(Char_t) * chis->GetSize(); // method of TArray baseclass
      } else if (his->InheritsFrom(TH3D::Class())) {
         TH3D *dhis = dynamic_cast<TH3D*>(his);
         fieldsize = sizeof(Double_t) * dhis->GetSize(); // method of TArray baseclass
      } else if (his->InheritsFrom(TH3F::Class())) {
         TH3F *fhis = dynamic_cast<TH3F*>(his);
         fieldsize = sizeof(Float_t) * fhis->GetSize(); // method of TArray baseclass
      } else if (his->InheritsFrom(TH3I::Class())) {
         TH3I *ihis = dynamic_cast<TH3I*>(his);
         fieldsize = sizeof(Int_t) * ihis->GetSize(); // method of TArray baseclass
      } else if (his->InheritsFrom(TH3S::Class())) {
         TH3S *shis = dynamic_cast<TH3S*>(his);
         fieldsize = sizeof(Short_t) * shis->GetSize(); // method of TArray baseclass
      } else if (his->InheritsFrom(TH3C::Class())) {
         TH3C *chis = dynamic_cast<TH3C*>(his);
         fieldsize = sizeof(Char_t) * chis->GetSize(); // method of TArray baseclass
      }
      fiObjectSize += fieldsize; // add heap field to object stack memory size
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
