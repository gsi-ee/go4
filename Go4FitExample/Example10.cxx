/* Go4Fit Example N10
   This is examples of using histogram as model component for another histogram
*/

#ifndef __CINT__

#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TApplication.h"

#include "../Go4Fit/TGo4FitMinuit.h"
#include "../Go4Fit/TGo4Fitter.h"
#include "../Go4Fit/TGo4FitDataHistogram.h"
#include "../Go4Fit/TGo4FitModelPolynom.h"
#include "../Go4Fit/TGo4FitModelGauss1.h"
#include "../Go4Fit/TGo4FitModelFromData.h"

void Example10();

int main(int argc, char **argv)
{
   TApplication theApp("Application", 0, 0);

   Example10();

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

void Example10() {
// create fitter and select function to fit
   TGo4Fitter fitter("Fitter", TGo4Fitter::ff_ML_Poisson, kTRUE);

   TH1D* histo1 = GetHistogram("hDeg120_P_c");            // get histogram from file
   TH1D* histo2 = GetHistogram("hDeg120_CND");            // get histogram from file

// add histogram to fitter, which should be fitted
   fitter.AddH1("data2", histo2, kTRUE, 2200., 2900.);

// create polynom of first order
   fitter.AddPolynomX("data2", "Pol", 1);

// add first histogram as component of second histogram
   fitter.AddModel("data2", new TGo4FitModelFromData("Model1",histo1,kTRUE) );

// create two gaussians
   fitter.AddGauss1("data2", "Gauss3", 2597., 14.);
   fitter.AddGauss1("data2", "Gauss4", 2717., 14.);

// perform all actions
   fitter.DoActions();

   // call fitter Draw method with options #data1,Gauss1,Gauss2
   fitter.Draw("#data2,Model1,Gauss3,Gauss4");
   fitter.Print("Pars");
}
