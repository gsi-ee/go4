// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum f�r Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

/* Go4Fit Examples
   This is small examples how to fit part of histogram
   with two gaussian and polynom function
*/

#ifndef __CINT__

#include "TH1.h"
#include "TFile.h"
#include "TApplication.h"

#include "TGo4Fitter.h"

void Example1();

int main(int argc, char **argv)
{
   TApplication theApp("Application", 0, 0);

   Example1();

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

void Example1()
{
// create fitter, select fit function and add standard actions list
   TGo4Fitter fitter("Fitter", TGo4Fitter::ff_ML_Poisson, kTRUE);

// add histogram to fitter, which should be fitted
   fitter.AddH1("data1", GetHistogram("hDeg120_P_c"), kTRUE, 2200., 2900.);

// create polynom of first order
   fitter.AddPolynomX("data1", "Pol", 1);

// create two gaussians
   fitter.AddGauss1("data1", "Gauss1", 2553., 15.);
   fitter.AddGauss1("data1", "Gauss2", 2672., 15.);

// execute all actions
   fitter.DoActions();

// draw data, full model and two gaussains on new canvas
   fitter.Draw("#data1,Gauss1,Gauss2");
}
