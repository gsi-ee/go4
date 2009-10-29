// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4COMEXECLINE_H
#define TGO4COMEXECLINE_H

#include "TGo4TaskHandlerCommand.h"
#include "TString.h"

/**
 * Command executes string as CINT macro
 */

class TGo4ComExecLine : public TGo4TaskHandlerCommand {
  public:

    TGo4ComExecLine();

    TGo4ComExecLine(const char* macro);

    virtual ~TGo4ComExecLine();

    Int_t ExeCom();

    /** access to the passwd. */
    const char* GetLine() const { return fxLine.Data(); }

     /** set the object name under command. */
    void SetLine(const char* name) { fxLine = name; }
  private:

    /** Contains text of macro. */
    TString fxLine ;

  ClassDef(TGo4ComExecLine,1)
};

#endif //TGO4COMSTART_H
