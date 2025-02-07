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

#ifndef TGO4USERSTOREPARAMETER_H
#define TGO4USERSTOREPARAMETER_H

#include "TGo4EventStoreParameter.h"

#include "TString.h"

/** @brief User store parameter
 * @ingroup go4_event
 */

class TGo4EventElement;

class TGo4UserStoreParameter : public TGo4EventStoreParameter {

  public:
    TGo4UserStoreParameter();

    TGo4UserStoreParameter(const char *name);

    virtual ~TGo4UserStoreParameter();

    /** basic method to printout object */
    void Print(Option_t *opt = "") const override;

    /** update contents of parameter class with external object. */
    Bool_t UpdateFrom(TGo4Parameter *rhs) override;

    /** User expression as string */
    const char *GetExpression() const { return fxExpression.Data(); }
    void SetExpression(const char *name) { fxExpression = name; }

  private:

    /** Any user defined expression as string */
    TString fxExpression;

  ClassDefOverride(TGo4UserStoreParameter,1)
};

#endif //TGO4USERSTOREPARAMETER_H
