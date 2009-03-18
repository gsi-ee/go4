#ifndef TGO4COMGETNAMESLIST_H
#define TGO4COMGETNAMESLIST_H

#include "TGo4AnalysisCommand.h"

/**
 * Requests for the current folder list of analysis
 * objects.
 */

class TGo4ComGetNamesList : public TGo4AnalysisCommand {
  public:

    TGo4ComGetNamesList();

    virtual ~TGo4ComGetNamesList();

    Int_t ExeCom();

  ClassDef(TGo4ComGetNamesList,1)
};

#endif //TGO4COMGETNAMESLIST_H
