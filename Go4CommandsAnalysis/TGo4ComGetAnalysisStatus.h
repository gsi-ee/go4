#ifndef TGO4COMGETANALYSISSTATUS_H
#define TGO4COMGETANALYSISSTATUS_H

#include "TGo4AnalysisCommand.h"

/**
 * Requests for the current analysis status. Status
 * contains all settings of the analysis steps.
 */

class TGo4ComGetAnalysisStatus : public TGo4AnalysisCommand {
  public:

    TGo4ComGetAnalysisStatus();

    virtual ~TGo4ComGetAnalysisStatus();

    Int_t ExeCom();

    ClassDef(TGo4ComGetAnalysisStatus,1)
};

#endif //TGO4COMGETANALYSISSTATUS_H
