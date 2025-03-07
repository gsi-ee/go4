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

#ifndef TGO4MAINTREE_H
#define TGO4MAINTREE_H

#include "TObject.h"

class TTree;
class TFile;

/**
 * @brief Singleton containing and managing the main tree of the go4 analysis.
 * @details All analysis steps may get and
 * save their events as branches of this tree. The treesource
 * and treestore instances are managing just one branch of
 * this tree for each of their subevents. Branches of this
 * tree might be saved into different root files.
 * @ingroup go4_event
 * @author J. Adamczewski
 * @since 16-feb-2001
 */
class TGo4MainTree : public TObject {
  friend class TGo4TreeSource;
  friend class TGo4TreeStore;

  protected:
    /** Access to the tree. For friends only, i.e
      * TreeSource and TreeStore. */
    TTree *GetTree() const { return fxTree; }

    TGo4MainTree();

  public:

    virtual ~TGo4MainTree();

    /** Standard go4 name of the main tree */
    static const char *fgcTREENAME; //!

    /** Standard suffix for file name */
    static const char *fgcFILENAME; //!

    /** Default compression level */
    static const Int_t fgiCOMPRESS; //!

    /** Default tree autosave size */
    static const Int_t fgiAUTOSAVESIZE; //!

    static TGo4MainTree *Instance();

    /** True if instance of tree already exists. Used by framework to check if MainTree
      * was created by any treestore/source instance, before the instance is
      * requested. Otherwise framework would create maintree without need. */
    static Bool_t Exists() { return fxInstance != nullptr; }

    Int_t GetCurrentIndex() const { return fiCurrentIndex; }

    /** Increments current index in the main tree by one.
      * Returns the value of the index after incrementing.
      * To be used by event loop controlling framework. */
    Int_t IncCurrentIndex() { return ++fiCurrentIndex; }

    void SetCurrentIndex(Int_t number = 0) { fiCurrentIndex = number; }

    Int_t GetMaxIndex();

    /** Set the tree autosave byte interval. May be changed on the fly. */
    void SetAutoSave(Int_t bytesinterval);

    /** Update the tree entry information without processing any branch.
      * i.e. increment the entry counter with all branches disabled. To be called by
      * framework once within the event loop. */
    void Update();

    /** Write tree content to file. */
    Int_t Write(const char *dummy = nullptr, Int_t option = 0, Int_t bufsize = 0) override;
    Int_t Write(const char *dummy = nullptr, Int_t option = 0, Int_t bufsize = 0) const override;

  private:

    static TGo4MainTree *fxInstance; //!

    TFile *fxFile{nullptr}; //!

    TTree *fxTree{nullptr}; //!

    /** Number of events stored in the Tree. Used to check if
      * the complete tree is already read. */
    Int_t fiMaxIndex{0};

    /** Index number of the current event. */
    Int_t fiCurrentIndex{0};

  ClassDefOverride(TGo4MainTree,1)
};

#endif //TGO4MAINTREE_H
