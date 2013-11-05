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

#ifndef TGO4COMDELETEOBJECT_H
#define TGO4COMDELETEOBJECT_H

#include "TGo4AnalysisObjectCommand.h"

/**
 * Deletes the object by name at the client. Object is firstly
 * searched from dynamic list; if it is not there, it is searched in the
 * root registry. Object will be deleted in all lists.
 * @author J. Adamczewski
 * @since 06-Jun-2001
 */
class TGo4ComDeleteObject : public TGo4AnalysisObjectCommand {
   public:
      TGo4ComDeleteObject();

      TGo4ComDeleteObject(const char* obname);

      virtual ~TGo4ComDeleteObject();

      virtual Int_t ExeCom();

  ClassDef(TGo4ComDeleteObject,1)
};

#endif //TGO4COMDELETEOBJECT_H
