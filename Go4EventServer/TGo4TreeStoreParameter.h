#ifndef TGO4TREESTOREPARAMETER_H
#define TGO4TREESTOREPARAMETER_H

#include "TGo4EventStoreParameter.h"
#include "TString.h"

class TGo4TreeStoreParameter : public TGo4EventStoreParameter {
  friend class TGo4TreeStore;

  public:
    TGo4TreeStoreParameter() ;

    TGo4TreeStoreParameter(const char* name,
                           Int_t splitlevel=1,
                           Int_t bufsize=64000,
                           const char* filename=0,
                           Int_t compression=5);

    virtual ~TGo4TreeStoreParameter() ;

    /** basic method to printout status information
      * on stdout; to be overridden by specific subclass */
    virtual Int_t PrintParameter(Text_t* buffer=0, Int_t buflen=0);

    /** update contents of paramter class with external object. */
    virtual Bool_t UpdateFrom(TGo4Parameter* rhs);

  private:
    Int_t fiSplit;
    Int_t fiBufsize;
    Int_t fiCompression;

    /** Filename for branch file.
      * If Null, the branch is stored in the maintree file. */
    TString fxBranchFile;

  ClassDef(TGo4TreeStoreParameter,1)
};

#endif //TGO4TREESTOREPARAMETER_H
