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

#ifndef TGO4USERSOURCEPARAMETER_H
#define TGO4USERSOURCEPARAMETER_H

#include "TGo4EventSourceParameter.h"

class TGo4UserSourceParameter : public TGo4EventSourceParameter {

  public:
    TGo4UserSourceParameter();

    TGo4UserSourceParameter(const char* name, const char* expr = "", Int_t port = 0);

    virtual ~TGo4UserSourceParameter();

    void SetPort(Int_t port) { fiPort=port; }
    Int_t GetPort() const { return fiPort; }

    /** User expression as string */
    const char* GetExpression() const { return fxExpression.Data(); }
    void SetExpression(const char* name) { fxExpression=name; }

    void Print(Option_t* = "") const override;

    /** update contents of parameter class with external object. */
    Bool_t UpdateFrom(TGo4Parameter* rhs) override;

  private:

    /** (Port) number. Optional and user defined.  */
    Int_t fiPort{0};

    /** Any user defined expression as string */
    TString fxExpression;

  ClassDefOverride(TGo4UserSourceParameter,1)
};

#endif //TGO4USERSOURCEPARAMETER_H
