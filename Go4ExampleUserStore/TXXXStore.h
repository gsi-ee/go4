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

#ifndef TEXAMPLEUSERSTORE_H
#define TEXAMPLEUSERSTORE_H

#include "TGo4EventStore.h"

class TTree;
class TFile;
class TGo4UserStoreParameter;
class TXXXEvent;

/**
 * Custom Event store
 * @since 2/2021
 */
class TXXXStore : public TGo4EventStore {

  public:

    TXXXStore();

    TXXXStore(TGo4UserStoreParameter *par);

    virtual ~TXXXStore();

    /** Stores eventelement event into the storage implementation. */
    Int_t Store(TGo4EventElement* event) override;

    /** Ignore store of parameter or one can provide some action */
    Int_t Store(TGo4Parameter*) override { return 0; }
    Int_t Store(TGo4Condition *) override { return 0; }
    Int_t Store(TGo4Fitter*) override { return 0; }
    Int_t Store(TFolder*) override { return 0; }

    /** Access to the Tree structure, for framework */
    TTree *GetTree() override { return fxTree; }

  private:

    TFile *fxFile{nullptr}; //! file
    TTree *fxTree{nullptr}; //! tree
    TXXXEvent *fxEvent{nullptr}; //! current event
    Bool_t fbBranchExists{kFALSE}; //! indicates if branch was created

  ClassDefOverride(TXXXStore,1)
};

#endif //TEXAMPLEUSERSTORE_H
