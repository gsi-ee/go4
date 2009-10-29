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

/* Go4Fit Example N4
   Same as small Example1, but for gaussian modelling new model component was constructed
   First way is to define user function and assign it to in TGo4FitModelFunction class
   In TModelTemplate class UserFunction method was redefined
*/

#ifndef __CINT__

#include "TH1.h"
#include "TFile.h"
#include "TApplication.h"

#include "../Go4Fit/TGo4Fitter.h"
#include "TModelTemplate.h"

void Example4();

int main(int argc, char **argv)
{
   TApplication theApp("Application", 0, 0);

   Example4();

   theApp.Run();

   return 0;
}

#endif

// routine to read histogram from examples file
TH1D* GetHistogram(const char* HistogramName) {
   TFile f1("histograms.root");
   TH1D* histo = (TH1D*) f1.Get(HistogramName);
   histo->SetDirectory(0);
   return histo;
}

void Example4() {
// create fitter, select fit function and add standard actions
  TGo4Fitter fitter("Fitter", TGo4Fitter::ff_ML_Poisson, kTRUE);

// add histogram to fitter, which should be fitted
   fitter.AddH1("data1", GetHistogram("hDeg120_P_c"), kTRUE, 2200., 2900.);

// create polynom of first order
   fitter.AddPolynomX( "data1", "Pol", 1);

// create model components, which uses new model class to calculate gaussian shape
   fitter.AddModel( "data1", new TModelTemplate("Gauss1",2,kTRUE) );
   fitter.AddModel( "data1", new TModelTemplate("Gauss2",2,kTRUE) );

// set initial values of user model parameters
   fitter.SetParValue("Gauss1.Par0",2553.);
   fitter.SetParValue("Gauss1.Par1",15.);
   fitter.SetParValue("Gauss2.Par0",2672.);
   fitter.SetParValue("Gauss2.Par1",15.);

// perform all actions
   fitter.DoActions();

// add output actions
   fitter.Draw ("#data1,Gauss1,Gauss2");
   fitter.Print("Pars");
}
