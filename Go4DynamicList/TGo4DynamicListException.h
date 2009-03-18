#ifndef TGO4DYNAMICLISTEXCEPTION_H
#define TGO4DYNAMICLISTEXCEPTION_H

#include "TGo4RuntimeException.h"
#include "TString.h"

class TGo4DynamicEntry;

class TGo4DynamicListException : public TGo4RuntimeException {

  public:

    TGo4DynamicListException (TGo4DynamicEntry* entry, const char* message,...);

    virtual ~TGo4DynamicListException();

    Int_t Handle ();

    TGo4DynamicListException(const TGo4DynamicListException &right);

    TGo4DynamicListException & operator = (const TGo4DynamicListException & right);

    /** Status message of last Process() call. */
    const char* GetStatusMessage() const { return fxStatusMessage.Data(); }

    /** Name of the throwing dynamic Entry. */
    const char* GetEntryName() const { return fxEntryName.Data(); }

    /** Classname of the throwing dynamic entry. */
    const char* GetEntryClass() const { return fxEntryClass.Data(); }

  protected:
    /** @supplierCardinality 1 */
    TGo4DynamicEntry * fxEntry;     //!

    TString fxStatusMessage;

    TString fxEntryName;

    TString fxEntryClass;
  private:
    TGo4DynamicListException();
};

#endif // #define TGO4TASKHANDLEREXCEPTION_H
