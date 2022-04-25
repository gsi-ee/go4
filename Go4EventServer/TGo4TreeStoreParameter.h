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

#ifndef TGO4TREESTOREPARAMETER_H
#define TGO4TREESTOREPARAMETER_H

#include "TGo4EventStoreParameter.h"
#include "TString.h"

class TGo4TreeStoreParameter : public TGo4EventStoreParameter {
  friend class TGo4TreeStore;

  public:
    TGo4TreeStoreParameter();

    TGo4TreeStoreParameter(const char* name,
                           Int_t splitlevel=1,
                           Int_t bufsize=64000,
                           const char* filename=0,
                           Int_t compression=5);

    virtual ~TGo4TreeStoreParameter();

    /** basic method to printout object */
    void Print(Option_t* = "") const override;

    /** update contents of parameter class with external object. */
    Bool_t UpdateFrom(TGo4Parameter* rhs) override;

  private:
    Int_t fiSplit{1};
    Int_t fiBufsize{64000};
    Int_t fiCompression{5};

    /** Filename for branch file.
      * If Null, the branch is stored in the maintree file. */
    TString fxBranchFile;

  ClassDefOverride(TGo4TreeStoreParameter,1)
};

#endif //TGO4TREESTOREPARAMETER_H
