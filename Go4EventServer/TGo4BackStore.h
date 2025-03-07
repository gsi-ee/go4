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

#ifndef TGO4BACKSTORE_H
#define TGO4BACKSTORE_H

#include "TGo4EventStore.h"

class TTree;
class TGo4EventElement;
class TGo4BackStoreParameter;

/**
 * @brief Event store to keep the last n events in a TTree which is _not_ saved to a file.
 * @details Allows to view the event structure from the tree and to perform TTree::Draw actions on the last events
 * by means of dynamic list. May contain ring buffer functionality.
 * @ingroup go4_event
 * @author J. Adamczewski
 * @since 4/2002
 */
class TGo4BackStore : public TGo4EventStore {

  public:

    TGo4BackStore();

    TGo4BackStore(TGo4BackStoreParameter* par);

    virtual ~TGo4BackStore();

    /** Stores eventelement event into the storage implementation. */
    Int_t Store(TGo4EventElement *event) override;

    /** Saves a calibration object correlated with the current event into the storage. */
    Int_t Store(TGo4Parameter *cali) override;

    /** Saves a condition correlated with the current event into the storage. */
    Int_t Store(TGo4Condition *conny) override;

    /** Saves a fitter correlated with the current event into the storage. */
    Int_t Store(TGo4Fitter *fitter) override;

    /** Saves a folder correlated with the current event into the storage. */
    Int_t Store(TFolder *fold) override;

    /** Access to the Tree structure, for framework */
    TTree *GetTree() override { return fxTree; }

    /** Dynamic list might reset backstore after processing.
     * If onlyclearflag is true, do not reset tree, but just
     * unmark the "backstore was reset" bit. */
    void Reset(Bool_t onlyclearflag = kFALSE);

    /** Standard go4 name of the branch used in all treestore
      * and treesource implementations. */
    static const char *fgcEVBRANCHNAME; //!

    /** Standard go4 suffix for tree name */
    static const char *fgcTREESUF; //!

    /** Number of bytes for autosave attempt of tree. This specifies when the
      * tree in memory will be reset, since we do not want to autosave anything here. */
    static const Int_t fgiMAXAUTOBYTES;

  private:

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

  ClassDefOverride(TGo4BackStore,1)
};

#endif //TGO4BACKSTORE_H
