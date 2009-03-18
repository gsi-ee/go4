#ifndef TGO4COMINITANALYSIS_H
#define TGO4COMINITANALYSIS_H

#include "TGo4AnalysisCommand.h"

/**
 * Executes the InitEventClasses Method of TGo4Analysis framework.
 */
class TGo4ComInitAnalysis : public TGo4AnalysisCommand {
  public:

    TGo4ComInitAnalysis();

    virtual ~TGo4ComInitAnalysis();

    Int_t ExeCom();

  ClassDef(TGo4ComInitAnalysis,1)
};

#endif //TGO4COMINITANALYSIS_H
