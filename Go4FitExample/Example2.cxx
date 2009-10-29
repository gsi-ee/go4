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

// Go4Fit Example N2


#ifndef __CINT__

#include "TH1.h"
#include "TFile.h"
#include "TApplication.h"

#include "../Go4Fit/TGo4Fitter.h"
#include "../Go4Fit/TGo4FitModelFunction.h"
#include "../Go4Fit/TGo4FitModelFormula.h"

void Example2();

int main(int argc, char **argv) {

   TApplication theApp("Application", 0, 0);

   Example2();

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

void Example2() {
// create fitter, select fit function and add standard actions list
   TGo4Fitter fitter("Fitter", TGo4Fitter::ff_ML_Poisson, kTRUE);

// add histogram to fitter, which should be fitted
   fitter.AddH1("data1", GetHistogram("hDeg120_P_c"), kTRUE, 2200., 2900.);

// create polynom of first order
   fitter.AddPolynomX( "data1", "Pol", 1);

// create models, which are used user function and formula to calculate gaussian shape
   TGo4FitModel* model1 = fitter.AddModel( "data1",
     new TGo4FitModelFunction("Gauss1","Example2Func.so","gaussian",2,kTRUE) );
   TGo4FitModel* model2 = fitter.AddModel( "data1",
     new TGo4FitModelFormula("Gauss2", "exp(-0.5*(x-Pos)*(x-Pos)/Width/Width)",2,kTRUE));

// set parameters names for models
   model1->SetParsNames("Ampl","Pos","Width");
   model2->SetParsNames("Ampl","Pos","Width");

// set initial values of user model parameters
   model1->SetParsValues(1.,2553.,15.);
   model2->SetParsValues(1.,2672.,15.);

// perform all actions
   fitter.DoActions();

// printing of fitter parameters
   fitter.Print("Pars");

// Draw data, model and two components
   fitter.Draw("#data1,Background,Gauss1,Gauss2");
//   fitter.Draw("#data1");
}
