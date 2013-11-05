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

#ifndef TGO4COMGETENVELOPE_H
#define TGO4COMGETENVELOPE_H

#include "TGo4AnalysisObjectCommand.h"

/**
 * Requests the object by name from the client. Object is firstly
 * taken from dynamic list; if it is not there, it is searched in the
 * root registry. Object will be send via data channel.
 * @author J. Adamczewski, S.Linev
 * @since April-2005
 */
class TGo4ComGetEnvelope : public TGo4AnalysisObjectCommand {
   public:

      TGo4ComGetEnvelope();

      TGo4ComGetEnvelope(const char* obname, const char* foldername);

      virtual ~TGo4ComGetEnvelope();

      virtual Int_t ExeCom();

   ClassDef(TGo4ComGetEnvelope,1)
};

#endif
