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

// Go4Fit Example N13

#ifndef __CINT__

#include "go4iostream.h"

#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TCutG.h"
#include "TApplication.h"
#include "TRandom.h"

#include "TGo4Fitter.h"
#include "TGo4FitDataHistogram.h"
#include "TGo4FitDataRidge.h"
#include "TGo4FitModelFormula.h"

void Example13();

int main(int argc, char **argv)
{
   TApplication theApp("Application", 0, 0);

   Example13();

   theApp.Run();

   return 0;
}

#endif

void AddRidge(TH2D* histo, double a, double k, double b,
              double xstart, int numsteps, int numpoints, double deviation)
{
  TRandom rnd(1234);

  double x = xstart;

  for(int i=0;i<numsteps;i++) {
     double y = a*exp(k*x) + b;

     double alf = TMath::ATan(k*a*exp(k*x)) + TMath::Pi()/2.;

     for(int j=0;j<numpoints;j++) {
        double radius = rnd.Gaus(0.,deviation);
        histo->Fill(x + radius*cos(alf), y + radius*sin(alf));
     }

     x = x + 0.001*sin(alf);
  }
}

TH2D* MakeRidgeHistogram()
{
  TH2D* histo = new TH2D("Histo","Ridge y = a*exp(k*x)+b",100,0.,10.,100,0.,10.);

  cout << "Generating histogram " << endl;

  AddRidge(histo, 10, -0.3,  2,  1.5, 10000, 100, 0.3);

  AddRidge(histo, 10, -0.5,  0,  0.5, 10000,  50, 0.2);

  AddRidge(histo, 10, -0.1, +2,  3.5, 10000,  50, 0.2);

  cout << "   Done " << endl;

  return histo;
}

TCutG* MakeCut()
{
   Float_t xx[7] = { 1.0, 4.0, 9.0, 9.5, 5.0, 2.0, 1.0 };
   Float_t yy[7] = { 8.0, 2.5, 2.0, 4.0, 6.0, 9.5, 8.0 };
   return new TCutG("RangeCut", 7, xx, yy);
}

void Example13()
{
// model histogram with ridge with exponential function
   TH2D* histo = MakeRidgeHistogram();
   new TCanvas("Canvas1");
   histo->Draw("LEGO2");

// create fitter, select fit function and not add standard actions list
   TGo4Fitter fitter("Fitter", TGo4Fitter::ff_chi_square, kFALSE);

// create data object to represent of two-dimensional histogram
// bins, which are less then 3 are excluded
// select middle ridge by TCutG object
   TGo4FitDataHistogram* datah = new TGo4FitDataHistogram("histogram",histo);
   datah->SetExcludeLessThen(3);
   datah->AddRangeCut(MakeCut());

// create ridge object, which select axis number 1 (y axis) as function of the rest coordinates
   TGo4FitDataRidge* ridge = new TGo4FitDataRidge("ridge",datah,1);
   fitter.AddData(ridge);

// create formula function to approximate ridge function
// specific range and initial values for parameters need to be set
   TGo4FitModelFormula* model = new TGo4FitModelFormula("RidgeFunc","parA*exp(parK*x)+parB",3,kFALSE);
   model->SetRange(0,2,8.);
   model->SetParsNames("parA","parK","parB");
   model->SetParsValues(9.,-0.2,1.2);
   fitter.AddModel("ridge", model);

// add minuit action
   fitter.AddSimpleMinuit();

// use memory buffers for data objects
   fitter.SetMemoryUsage(1);

// execute actions
   fitter.DoActions();

// print parameters values
   fitter.Print("Pars");

// draw results of fit
   fitter.Draw("#ridge");
}
