#ifndef TGO4FILESTORE_H
#define TGO4FILESTORE_H

#include "TGo4EventStore.h"

class TFile;
class TTree;

class TGo4EventCalibration;
class TGo4EventElement;
class TGo4FileStoreParameter;

/**
 * Event store which fills entries to an own root TTree in a TFile. In contrast to TreeStore, we do not use the main
 * go4 tree, but an independent one which need not
 * correspond with the entry indices of all previous
 * events. Disadvantage: difficult to correlate to input
 * events; advantage: one entry of this tree may be composed
 * from several subsequent input event entries...
 * @author J. Adamczewski
 * @since 1/2001
 */
class TGo4FileStore : public TGo4EventStore {
  public:

    TGo4FileStore();

    TGo4FileStore(const char* name,
                  Int_t splitlevel=1,
                  Int_t compression=1,
                  Bool_t overwrite=kFALSE,
                  Int_t autosavesize=10000000,
                  Int_t bufsize=64000);

    TGo4FileStore(TGo4FileStoreParameter* par);

    virtual ~TGo4FileStore();

    /** Stores eventelement event into the storage implementation.
      * May be a file, a root tree, a socket to a viewer... */
    virtual Int_t Store(TGo4EventElement* event);

    /** Saves a calibration object correllated with the current event into the storage. */
    virtual Int_t Store(TGo4Parameter* cali);

    /** Saves a condition correllated with the current event into the storage. */
    virtual Int_t Store(TGo4Condition* conny);

    /** Saves a fitter correllated with the current event into the storage. */
    virtual Int_t Store(TGo4Fitter* fitter);

    /** Saves a complete objects folder correllated with the current event into the storage. */
    virtual Int_t Store(TFolder* fold);

    /** Set the file compression level. May be changed on the fly. */
    void SetCompression(Int_t comp);

    /** Set the tree autosave byte interval. May be changed on the fly. */
    void SetAutoSave(Int_t bytesinterval);

    /** Standard go4 name of the branch used in all treestore
      * and treesource implementations. */
    static const Text_t fgcEVBRANCHNAME[]; //!

    /** Standard suffix for file name */
    static const Text_t fgcFILESUF[]; //!

    /** Standard go4 suffix for tree name */
    static const Text_t fgcTREESUF[]; //!

    /** size in bytes for root tree-filesplit mechanism. */
    static const Int_t fgiFILESPLITSIZE;

    /** Access to the Tree structure, for framework */
    virtual TTree* GetTree() { return fxTree; }

  private:

    /** used by all Store methods to write with referencing event number in name */
    void WriteToStore(TNamed* ob);

    TFile* fxFile; //!

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

  ClassDef(TGo4FileStore,1)
};

#endif //TGO4FILESTORE_H
