// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

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

    TGo4FileStore(const char *name,
                  Int_t splitlevel=1,
                  Int_t compression=1,
                  Bool_t overwrite=kFALSE,
                  Int_t autosavesize=10000, // positive: entries, negative: bytes
                  Int_t bufsize=64000);

    TGo4FileStore(TGo4FileStoreParameter* par);

    virtual ~TGo4FileStore();

    /** Stores eventelement event into the storage implementation.
      * May be a file, a root tree, a socket to a viewer... */
    Int_t Store(TGo4EventElement* event) override;

    /** Saves a calibration object correlated with the current event into the storage. */
    Int_t Store(TGo4Parameter *cali) override;

    /** Saves a condition correlated with the current event into the storage. */
    Int_t Store(TGo4Condition *conny) override;

    /** Saves a fitter correlated with the current event into the storage. */
    Int_t Store(TGo4Fitter* fitter) override;

    /** Saves a complete objects folder correlated with the current event into the storage. */
    Int_t Store(TFolder *fold) override;

    /** Set the file compression level. May be changed on the fly. */
    void SetCompression(Int_t comp);

    /** Set the tree autosave interval. May be changed on the fly.
     *  for new ROOT versions: interval>0  means number of entries, interval<0: number of bytes*/
    void SetAutoSave(Int_t interval);

    static void SetMaxTreeSize(Long64_t sz);

    static Long64_t GetMaxTreeSize();

    /** Standard go4 name of the branch used in all treestore
      * and treesource implementations. */
    static const char *fgcEVBRANCHNAME; //!

    /** Standard suffix for file name */
    static const char *fgcFILESUF; //!

    /** Standard go4 suffix for tree name */
    static const char *fgcTREESUF; //!

    /** size in bytes for root tree-filesplit mechanism. */
    static Long64_t fgiFILESPLITSIZE;

    /** Access to the Tree structure, for framework */
    TTree *GetTree() override { return fxTree; }

  private:

    /** used by all Store methods to write with referencing event number in name */
    void WriteToStore(TNamed *ob);

    TFile *fxFile{nullptr}; //!

    TTree *fxTree{nullptr}; //!

    /** True if branch already exists. Used for automatic creation
      * of new event branch within Store method. */
    Bool_t fbBranchExists{kFALSE};

    /** Points to event structure to be filled into branch. */
    TGo4EventElement *fxEvent{nullptr}; //!

    /** TBranch splitlevel. */
    Int_t fiSplit{0};

    /** buffer size for tree branch. */
    Int_t fiBufsize{0};

    /** counter of filled events. */
    Int_t fiFillCount{0};

  ClassDefOverride(TGo4FileStore,1)
};

#endif //TGO4FILESTORE_H
