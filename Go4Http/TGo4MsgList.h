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

#ifndef TGO4MSGLIST_H
#define TGO4MSGLIST_H

#include "TNamed.h"
#include "TList.h"
#include "TObjString.h"

class TGo4MsgList : public TNamed {

   protected:

      TList      fMsgs;    //! list of remained messages
      Int_t      fLimit;   //! max number of stored messages
      Long64_t   fCounter; //! current message id

   public:

      TGo4MsgList();
      TGo4MsgList(const char* name, const char* title);
      virtual ~TGo4MsgList();

   ClassDef(TGo4MsgList, 1);
};


#endif
