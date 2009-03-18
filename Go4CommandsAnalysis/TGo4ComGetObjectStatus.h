#ifndef TGO4COMGETOBJECTSTATUS_H
#define TGO4COMGETOBJECTSTATUS_H

#include "TGo4AnalysisObjectCommand.h"

/**
 * Requests the object status by name from the client. Used to retrieve current information on
 * object without transporting it entirely.
 * @author J. Adamczewski
 * @since 06-Jun-2001
 */
class TGo4ComGetObjectStatus : public TGo4AnalysisObjectCommand {
  public:

    TGo4ComGetObjectStatus();

    TGo4ComGetObjectStatus(const char* obname);

    virtual ~TGo4ComGetObjectStatus();

    Int_t ExeCom();

  ClassDef(TGo4ComGetObjectStatus,1)
};

#endif //TGO4COMGETOBJECTSTATUS_H
