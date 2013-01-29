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

// Go4Fit Examples N3

#ifndef __CINT__

#include "Riostream.h"
#include "TH1.h"
#include "TFile.h"
#include "TApplication.h"

#include "TGo4Fitter.h"
#include "TGo4FitModelFunction.h"

void Example3();

int main(int argc, char **argv)
{
   TApplication theApp("Application", 0, 0);

   Example3();

   theApp.Run();

   return 0;
}

extern "C" Double_t gaussian_(Double_t *axis, Double_t *pars);

// routine to read histogram from examples file
TH1D* GetHistogram(const char* HistogramName)
{
   TFile* f1 = TFile::Open("histograms.root");
   if (f1==0) return 0;
   TH1D* histo = (TH1D*) f1->Get(HistogramName);
   if (histo) histo->SetDirectory(0);
   return histo;
}

void Example3()
{
// create fitter, select fit function and add standard actions list
   TGo4Fitter fitter("Fitter", TGo4Fitter::ff_ML_Poisson, kTRUE);

// add histogram to fitter, which should be fitted
   fitter.AddH1("data1", GetHistogram("hDeg120_P_c"), kTRUE, 2200., 2900.);

// create polynom of first order
   fitter.AddPolynomX( "data1", "Pol", 1);

// create model, which uses user function to calculate gaussian shape
   fitter.AddModel( "data1", new TGo4FitModelFunction("Gauss1",&gaussian_,2,kTRUE) );
   fitter.AddModel( "data1", new TGo4FitModelFunction("Gauss2",&gaussian_,2,kTRUE) );

// set initial values of user model parameters
   fitter.SetParValue("Gauss1.Par0",2553.);
   fitter.SetParValue("Gauss1.Par1",15.);
   fitter.SetParValue("Gauss2.Par0",2672.);
   fitter.SetParValue("Gauss2.Par1",15.);

// add output actions
   fitter.AddOutputAction("Draw","#data1,Gauss1,Gauss2");
   fitter.AddOutputAction("Print","Pars");

// perform all actions
   fitter.DoActions();
}

#else

void Example3()
{
   std::cout << "This example uses linked FORTRAN library," << std::endl;
   std::cout << "therefore it can not be run in CINT" << std::endl;
}

#endif

