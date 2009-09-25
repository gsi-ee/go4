#ifndef TGO4BACKSTORE_H
#define TGO4BACKSTORE_H

#include "TGo4EventStore.h"

class TTree;
class TGo4EventElement;
class TGo4BackStoreParameter;

/**
 * Event store to keep the last n events in a TTree which is _not_ saved to a file. Allows to
 * view the event structure from the tree and to perform TTree::Draw actions on the last events
 * by means of dynamic list. May contain ring buffer functionality.
 * @author J. Adamczewski
 * @since 4/2002
 */
class TGo4BackStore : public TGo4EventStore {

  public:

    TGo4BackStore();

    TGo4BackStore(TGo4BackStoreParameter* par);

    virtual ~TGo4BackStore();

    /** Stores eventelement event into the storage implementation. */
    virtual Int_t Store(TGo4EventElement* event);

    /** Saves a calibration object correllated with the current event into the storage. */
    virtual Int_t Store(TGo4Parameter* cali);

    /** Saves a condition correllated with the current event into the storage. */
    virtual Int_t Store(TGo4Condition* conny);

    /** Saves a fitter correllated with the current event into the storage. */
    virtual Int_t Store(TGo4Fitter* fitter);

    /** Saves a folder correllated with the current event into the storage. */
    virtual Int_t Store(TFolder* fold);

    /** Access to the Tree structure, for framework */
    virtual TTree* GetTree() { return fxTree; }

    /** Dynamic list might reset backstore after processing.
     * If onlyclearflag is true, do not reset tree, but just
     * unmark the "backstore was reset" bit. */
    void Reset(Bool_t onlyclearflag=kFALSE);

    /** Standard go4 name of the branch used in all treestore
      * and treesource implementations. */
    static const char* fgcEVBRANCHNAME; //!

    /** Standard go4 suffix for tree name */
    static const char* fgcTREESUF; //!

    /** Number of bytes for autosave attempt of tree. This specifies when the
      * tree in memory will be reset, since we do not want to autosave anything here. */
    static const Int_t fgiMAXAUTOBYTES;

  private:

    TTree * fxTree; //!

    /** True if branch already exists. Used for automatic creation
      * of new event branch within Store method. */
    Bool_t fbBranchExists;

    /** Points to event structure to be filled into branch. */
    TGo4EventElement * fxEvent; //!

    /** TBranch splitlevel. */
    Int_t fiSplit;

    /** buffer size for tree branch. */
    Int_t fiBufsize;

    /** counter of filled events. */
    Int_t fiFillCount;

  ClassDef(TGo4BackStore,1)
};

#endif //TGO4BACKSTORE_H
