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

#ifndef TGO4COMSETPROTECTIONS_H
#define TGO4COMSETPROTECTIONS_H

#include "TGo4AnalysisObjectCommand.h"

/**
 * Change protections against clearing (reset) or deletion, etc.
 * for target objects
 * @author J. Adamczewski
 * @since 03-Dec-2004
 */
class TGo4ComSetProtections : public TGo4AnalysisObjectCommand {
  public:

    TGo4ComSetProtections();

    TGo4ComSetProtections(const char* obname, const char* flags);

    Int_t ExeCom() override;

    /** Set internals of this command from specification of external
     * remote command (command container). Must be overloaded
     * for each specific command! */
    void Set(TGo4RemoteCommand* remcon) override;

    void SetFlags(const char* flags) { fxFlags = flags; }

  private:

    TString fxFlags;

   ClassDefOverride(TGo4ComSetProtections,1)
};


#endif //TGO4COMSETPROTECTIONS_H
