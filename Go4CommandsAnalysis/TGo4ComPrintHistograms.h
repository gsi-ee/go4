#ifndef TGO4COMPRINTHISTOGRAMS_H
#define TGO4COMPRINTHISTOGRAMS_H

#include "TGo4AnalysisCommand.h"

/**
 * Printout all histogram statisticx on the local analysis terminal.
 * May send this information to the gui later if option fbLocal is set to kFALSE.
 */
class TGo4ComPrintHistograms : public TGo4AnalysisCommand {
  public:

    TGo4ComPrintHistograms();

    virtual ~TGo4ComPrintHistograms();

    Int_t ExeCom();

  ClassDef(TGo4ComPrintHistograms,1)
};
#endif //TGO4COMPRINTHISTOGRAMS_H
