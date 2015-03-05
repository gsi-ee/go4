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

#ifndef TGO4DABC_H
#define TGO4DABC_H

class THttpServer;

class TGo4Dabc {
   protected:

      static THttpServer* gHttpServer;

   public:

      static THttpServer* GetHttpServer() { return gHttpServer; }

      static bool CreateEngine(const char* name);
};

#endif
