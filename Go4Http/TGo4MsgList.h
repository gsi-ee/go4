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

#ifndef TGO4MSGLIST_H
#define TGO4MSGLIST_H

#include "TNamed.h"
#include "TList.h"
#include "TObjString.h"

class TGo4MsgList : public TNamed {

   protected:

      TList      fMsgs;       //  list of remained messages, stored as TObjString
      Int_t      fLimit{0};   //  max number of stored messages
      Long64_t   fCounter{0}; //  current message id
      TList      fSelect;     //! temporary list used for selection
      TObjString fStrCounter; //! current id stored in the string

   public:

      TGo4MsgList();
      TGo4MsgList(const char *name, const char *title, Int_t limit = 1000);
      virtual ~TGo4MsgList();

      void SetLimit(Int_t limit)
      {
         fLimit = limit > 0 ? limit : 1;
         AddMsg(nullptr);
      }
      Int_t GetLimit() const { return fLimit; }

      Int_t GetCounter() const { return fCounter; }

      void AddMsg(const char *msg);

      TList *Select(Int_t max = 0, Long64_t id = 0);

   ClassDefOverride(TGo4MsgList, 1); // Custom message list for web server

};

#endif
