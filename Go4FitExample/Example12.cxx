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

// Go4Fit Example N12


#ifndef __CINT__

#include "TGraph.h"
#include "TApplication.h"

#include "TGo4Fitter.h"
#include "TGo4FitDataGraph.h"

void Example12();

int main(int argc, char **argv)
{
   TApplication theApp("Application", 0, 0);

   Example12();

   theApp.Run();

   return 0;
}

#endif

// routine to generate example graph function
TGraph* MakeGraph()
{
  TGraph* gr = new TGraph(40);
  for(Int_t i=0;i<40;i++) {
    Double_t x = (i+1.)/40.;
    Double_t y = 5 - 0.5*x + x*x;
    if ((x>0.3) && (x<0.7))
      y+= 10.*(1. - 25.*(x-.5)*(x-.5));
    gr->SetPoint(i, x, y);
  }
  return gr;

}

void Example12()
{
// create fitter, select fit function and add standard actions list
   TGo4Fitter fitter("Fitter", TGo4Fitter::ff_ML_Poisson, kTRUE);

// add histogram to fitter, which should be fitted
   fitter.AddGraph("data1", MakeGraph(), kTRUE);

// add polynom of 2 order for background approx
   fitter.AddPolynomX("data1", "BackgrPol", 2);

// add polynom of 2 order for background approx
   fitter.AddPolynomX("data1", "LinePol", 2, 123, 0.3, 0.7);

// perform all actions
   fitter.DoActions();

// printing of fitter parameters
   fitter.Print("Pars");

// Draw data ("-" means without full model), Background and line (has group 123)
   fitter.Draw("#data1-, Background, Group123");
}
