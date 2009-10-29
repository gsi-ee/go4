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

#ifndef TGO4COMCLEAROBJECT_H
#define TGO4COMCLEAROBJECT_H

#include "TGo4AnalysisObjectCommand.h"

/**
 * Calls Clear() method of client object, if object of this name exists. Object will
 * be searched in dynamic lists first, then in entire root registry. Useful to clear
 * histogram contents, e.g.
 * @author J. Adamczewski
 * @since 06-Jun-2001
 */
class TGo4ComClearObject : public TGo4AnalysisObjectCommand {
  public:

    TGo4ComClearObject();

    TGo4ComClearObject(const char* obname);

    virtual ~TGo4ComClearObject();

    Int_t ExeCom();

  ClassDef(TGo4ComClearObject,1)
};

#endif //TGO4COMCLEAROBJECT_H
