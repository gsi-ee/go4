#ifndef TGO4TREESOURCE_H
#define TGO4TREESOURCE_H

#include "TGo4EventSource.h"

class TGo4TreeSourceParameter;
class TGo4MainTree;
class TGo4EventElement;
class TBranch;
class TTree;

/**
 * Raw event source which reads entries from a root TTree.
 * We use one single tree for all generations of events,
 * which is kept by the go4 maintree class.
 * @author J. Adamczewski
 * @since 1/2001
 */
class TGo4TreeSource : public TGo4EventSource {
  public:
    TGo4TreeSource();

    TGo4TreeSource(const char* name);

    virtual ~TGo4TreeSource();

    TGo4TreeSource(TGo4TreeSourceParameter* par);

    /** Fill the destination event dest from the tree.
      * If end of tree is reached, the current event counter
      * is reset and a kFALSE value is returned. */
    Bool_t BuildEvent(TGo4EventElement* dest);

  private:

    /** Reference to external tree singleton wrapper instance.
      * Is initialized once at the start and used to access
      * the singleton member functions. */
    TGo4MainTree* fxSingletonTree; //!

    /** Reference to external root tree singleton. */
    TTree * fxTree;//!

    /** The branch representing our source. */
    TBranch * fxBranch; //!

    Int_t Open();

  ClassDef(TGo4TreeSource,1)
};

#endif //TGO4TREESOURCE_H
