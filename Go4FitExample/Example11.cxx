// Go4Fit Example N11


#ifndef __CINT__

#include "TH1.h"
#include "TFile.h"
#include "TApplication.h"

#include "../Go4Fit/TGo4Fitter.h"
#include "../Go4Fit/TGo4FitPeakFinder.h"

void Example11();

int main(int argc, char **argv) {

   TApplication theApp("Application", 0, 0);

   Example11();

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

void Example11() {
// create fitter, select fit function and not add standard actions list
   TGo4Fitter fitter("Fitter", TGo4Fitter::ff_ML_Poisson, kTRUE);

// add histogram to fitter, which should be fitted
   fitter.AddH1("data1", GetHistogram("hDeg120_P_c"), kTRUE, 2200., 2900.);


// Add peak finder for data1, which not remove previous models and will use 1-order
// polynom for background
   TGo4FitPeakFinder* finder = new TGo4FitPeakFinder("Finder", "data1", kFALSE, 1);

// setup parameters for first peak finder.
// Here first - amplitude threshold relative to amplitude maximum,
// and minimum and maximum line width
   finder->SetupForFirst(.3, 5., 30.);

// add peak finder action at the beginning
   fitter.AddActionAt(finder, 0);

// perform all actions
// kTRUE means, that action is able to add/delete components to fitter
   fitter.DoActions(kTRUE);

// print models parameters in lines sorting
   fitter.Print("Lines");

// Draw data and model
   fitter.Draw("#data1");
}
