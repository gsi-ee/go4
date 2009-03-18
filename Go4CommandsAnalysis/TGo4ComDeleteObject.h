#ifndef TGO4COMDELETEOBJECT_H
#define TGO4COMDELETEOBJECT_H

#include "TGo4AnalysisObjectCommand.h"

/**
 * Deletes the object by name at the client. Object is firstly
 * searched from dynamic list; if it is not there, it is searched in the
 * root registry. Object will be deleted in all lists.
 * @author J. Adamczewski
 * @since 06-Jun-2001
 */
class TGo4ComDeleteObject : public TGo4AnalysisObjectCommand {
  public:
    TGo4ComDeleteObject();

    TGo4ComDeleteObject(const char* obname);

    virtual ~TGo4ComDeleteObject();

    Int_t ExeCom();

  ClassDef(TGo4ComDeleteObject,1)
};

#endif //TGO4COMDELETEOBJECT_H
