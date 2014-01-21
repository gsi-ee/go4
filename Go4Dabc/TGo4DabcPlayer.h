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


#ifndef TGO4DABCPLAYER_H
#define TGO4DABCPLAYER_H

#include "root/Player.h"

#include "TGo4AnalysisSniffer.h"

class TGo4DabcPlayer : public root::Player,
                       public TGo4AnalysisSniffer {

   protected:

      virtual void InitializeHierarchy();

      virtual int ExecuteCommand(dabc::Command cmd);

      virtual int ProcessGetBinary(TRootSniffer* sniff, dabc::Command cmd);

   public:
      TGo4DabcPlayer(const std::string& name, dabc::Command cmd = 0);

      virtual ~TGo4DabcPlayer();

      virtual const char* ClassName() const { return "TGo4DabcPlayer"; }

      virtual void RatemeterUpdate(TGo4Ratemeter*);

      virtual void StatusMessage(int level, const TString&);

      /** Method called by logger with every string, going to output */
      virtual void SetTitle(const char* title = "");

};

#endif
