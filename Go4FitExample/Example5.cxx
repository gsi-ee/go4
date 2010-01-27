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

/* Go4Fit Example N5
   This is example how to fit one histogram and then apply fitter for another histogram
*/

#ifndef __CINT__

#include "TH1.h"
#include "TFile.h"
#include "TApplication.h"

#include "TGo4FitMinuit.h"
#include "TGo4Fitter.h"
#include "TGo4FitDataHistogram.h"

void Example5();

int main(int argc, char **argv)
{
   TApplication theApp("Application", 0, 0);

   Example5();

   theApp.Run();

   return 0;
}

#endif


// routine to read histogram from examples file
TH1D* GetHistogram(const char* HistogramName)
{
   TFile* f1 = TFile::Open("histograms.root");
   if (f1==0) return 0;
   TH1D* histo = (TH1D*) f1->Get(HistogramName);
   if (histo) histo->SetDirectory(0);
   return histo;
}

void Example5()
{
// create fitter, select fit function and add standard actions list
   TGo4Fitter fitter("Fitter", TGo4Fitter::ff_ML_Poisson, kTRUE);

// add histogram to fitter, which should be fitted
   TGo4FitDataHistogram *data = fitter.AddH1("data1", GetHistogram("hDeg120_P_c"), kTRUE, 2200., 2900.);

// create polynom of first order
   fitter.AddPolynomX( "data1", "Pol", 1);

// create two gaussians
   fitter.AddGauss1( "data1", "Gauss1",2553.,15.);
   fitter.AddGauss1( "data1", "Gauss2",2672.,15.);

// perform all actions
   fitter.DoActions();

// Draw results
   fitter.Draw("#data1,Gauss1,Gauss2");


// Up to here same as Example1
// Now we modify fitter and data to fit

// add two mode components (peaks) to fitter
   fitter.AddGauss1( "data1", "Gauss3", 2597., 14.);
   fitter.AddGauss1( "data1", "Gauss4", 2717., 14.);

// provide new histogram to data object
   data->SetHistogram(GetHistogram("hDeg120_CND"),kTRUE);

// perform all actions
   fitter.DoActions();

// remove output actions and add new
   fitter.Draw("#data1,Gauss1,Gauss2,Gauss3,Gauss4");
   fitter.Print("Pars");
}
