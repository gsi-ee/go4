#include "TGo4FitterAction.h"


TGo4FitterAction::TGo4FitterAction() : TGo4FitNamed() {
}

TGo4FitterAction::TGo4FitterAction(const char* Name, const char* Title, TNamed* Owner) :
   TGo4FitNamed(Name,Title,Owner) {
}

TGo4FitterAction::~TGo4FitterAction() {
}
