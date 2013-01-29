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

#include "TGo4FitterOutput.h"

#include "Riostream.h"
#include "TVirtualPad.h"

#include "TGo4FitterAbstract.h"

TGo4FitterOutput::TGo4FitterOutput() : TGo4FitterAction(), fxCommand(), fxOptions(), fxPad(0) {
}

TGo4FitterOutput::TGo4FitterOutput(const char* Command, const char* Options) :
    TGo4FitterAction(Command,"Output action for fitter"), fxCommand(Command), fxOptions(Options), fxPad(0)  {
}

TGo4FitterOutput::~TGo4FitterOutput() {
}

void TGo4FitterOutput::DoAction(TGo4FitterAbstract* Fitter) {
   if (Fitter==0) return;
   if (fxCommand=="Print") Fitter->Print(fxOptions.Data()); else
   if (fxCommand=="Draw") {
     TVirtualPad* old = gPad;
     if (fxPad!=0) fxPad->cd();
     Fitter->Draw(fxOptions.Data());
     if (fxPad) fxPad->Update();
     if (old!=0) old->cd();
   }
}

void TGo4FitterOutput::Print(Option_t* option) const {
   TGo4FitterAction::Print(option);
   std::cout << "  command: " << fxCommand;
   if (fxOptions.Length()>0) std::cout << "  options: " << fxOptions;
   std::cout << std::endl;
}
