#ifndef TGO4COMPRINTCONDITIONS_H
#define TGO4COMPRINTCONDITIONS_H

#include "TGo4AnalysisCommand.h"

/**
 * Printout all condition counters as bar diagram on the local analysis terminal.
 * May send this information to the gui later if option local is kFALSE.
 */
class TGo4ComPrintConditions : public TGo4AnalysisCommand {
  public:

    TGo4ComPrintConditions();

    virtual ~TGo4ComPrintConditions();

    Int_t ExeCom();

  ClassDef(TGo4ComPrintConditions,1)
};

#endif //TGO4COMPRINTCONDITIONS_H
