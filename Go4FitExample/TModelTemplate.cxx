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

/* template for user model class
   The only function, which should be redefined by used - UserFunction()
   In given example constructor provides a general interface to create
   several parameters. Amplitude parameters can be add, if necessary.
   Any kind of other constructors with predefine parameters set and names can be invented
   Here user function is Gaussian
*/


#include "TString.h"
#include "TMath.h"
#include "TModelTemplate.h"

TModelTemplate::TModelTemplate(const char* iName, Int_t iNPars, Bool_t AddAmplitude) :
   TGo4FitModel(iName,"Template for user model",AddAmplitude) 
{
   for (Int_t n=0;n<iNPars;n++) {
       TString s("Par"); s+=n;
       NewParameter(s.Data(),"model template parameter",0.);
   }
}

Double_t TModelTemplate::UserFunction(Double_t* Coordinates, Double_t* Parameters) 
{
   Double_t pos = Parameters[0];
   Double_t width = Parameters[1];
   Double_t x = Coordinates[0];
   return TMath::Exp(-0.5*(x-pos)*(x-pos)/width/width);
}
