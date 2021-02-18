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

#ifndef TXXXKSTORE_H
#define TXXXKSTORE_H

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
    virtual Int_t Store(TGo4EventElement* event);

    /** Ignore store of parameter or one can provide some action */
    virtual Int_t Store(TGo4Parameter*) { return 0; }
    virtual Int_t Store(TGo4Condition*) { return 0; }
    virtual Int_t Store(TGo4Fitter*) { return 0; }
    virtual Int_t Store(TFolder*) { return 0; }

    /** Access to the Tree structure, for framework */
    virtual TTree* GetTree() { return fxTree; }

  private:

    TFile *fxFile; //! file
    TTree *fxTree; //! tree

    TXXXEvent *fxEvent; //! current event
    Bool_t fbBranchExists; //! indicates if branch was created

  ClassDef(TXXXStore,1)
};

#endif //TGO4BACKSTORE_H
