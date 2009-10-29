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

#ifndef TGO4COMGETOBJECTSTATUS_H
#define TGO4COMGETOBJECTSTATUS_H

#include "TGo4AnalysisObjectCommand.h"

/**
 * Requests the object status by name from the client. Used to retrieve current information on
 * object without transporting it entirely.
 * @author J. Adamczewski
 * @since 06-Jun-2001
 */
class TGo4ComGetObjectStatus : public TGo4AnalysisObjectCommand {
  public:

    TGo4ComGetObjectStatus();

    TGo4ComGetObjectStatus(const char* obname);

    virtual ~TGo4ComGetObjectStatus();

    Int_t ExeCom();

  ClassDef(TGo4ComGetObjectStatus,1)
};

#endif //TGO4COMGETOBJECTSTATUS_H
