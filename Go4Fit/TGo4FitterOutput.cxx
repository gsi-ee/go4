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
   cout << "  command: " << fxCommand;
   if (fxOptions.Length()>0) cout << "    options: " << fxOptions << endl;
                        else cout << endl;
}
