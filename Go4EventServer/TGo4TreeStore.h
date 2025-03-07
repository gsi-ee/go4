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

#ifndef TGO4TREESTORE_H
#define TGO4TREESTORE_H

#include "TGo4EventStore.h"

class TFile;
class TTree;
class TBranch;
class TGo4MainTree;
class TGo4EventElement;
class TGo4Parameter;
class TGo4EventCalibration;
class TGo4TreeStoreParameter;

/**
 * @brief Go4 TTree storage
 * @details Storage instance using the central main go4 tree which
 * is accessible as singleton. Each treestore instance
 * adds at least one new branch to that tree and fills
 * it with the data at the event pointer. Branch associated
 * with this tree store may be written to the main tree file,
 * or be located in another file which is specified by the
 * filename parameter of ctor.
 * @ingroup go4_event
 * @author J. Adamczewski
 * @since 1/2001
 */
class TGo4TreeStore : public TGo4EventStore {
  public:

    TGo4TreeStore(const char *name,
                  TGo4EventElement *event,
                  Int_t splitlevel = 1,
                  Int_t bufsize = 64000,
                  const char *filename = nullptr,
                  Int_t compression = 5);

    TGo4TreeStore();

    TGo4TreeStore(TGo4TreeStoreParameter* par, TGo4EventElement *event);

    virtual ~TGo4TreeStore();

    /** Expose all Store methods to avoid compiler warnings, A.Neiser */
    using TGo4EventStore::Store;

    /** Stores eventelement event into the storage implementation.
      * May be a file, a root tree, a socket to a viewer... */
    Int_t Store(TGo4EventElement *event) override;

    /** Set the file compression level. May be changed on the fly. */
    void SetCompression(Int_t comp);

    /** Saves a calibration object correlated with the current event into the storage.
      * This method is optional, i.e. it needs not to be implemented for all subclasses. */
    Int_t Store(TGo4Parameter *cali) override;

    /** Standard suffix for file name */
    static const char *fgcFILESUF; //!

  private:

    /** Optional file where our branches are located. By
      * default, branches are written to the same file as the main tree. */
    TFile *fxFile{nullptr}; //!

    /** Reference to external tree singleton wrapper instance.
      * Is initialized once at the start and used to access
      * the singleton member functions. */
    TGo4MainTree *fxSingletonTree{nullptr}; //!

    /** Reference to the external main tree. */
    TTree *fxTree{nullptr}; //!

    /** The branch representing our store. */
    TBranch *fxBranch{nullptr}; //!

    /** Points to event structure to be filled into branch. */
    TGo4EventElement *fxEvent{nullptr}; //!

    /** Points to calibration structure to be filled into branch. */
    TGo4EventCalibration *fxCalibration{nullptr}; //!

    /** Class name of the event associated with this store. */
    TString fcEventClass; //!

    /** TBranch splitlevel. */
    Int_t fiSplit{0};

    /** buffer size for tree branch. */
    Int_t fiBufsize{0};

  ClassDefOverride(TGo4TreeStore,1)
};

#endif //TGO4TREESTORE_H
