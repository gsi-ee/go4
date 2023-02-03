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

#ifndef TGO4ROOTBROWSERPROXY_H
#define TGO4ROOTBROWSERPROXY_H

#include "TGo4Proxy.h"

class TGo4BrowserProxy;
class TGo4Picture;
class TGo4Condition;
class TPad;
class TCanvas;

class TGo4RootBrowserProxy : public TGo4Proxy {
   public:
      TGo4RootBrowserProxy(TGo4BrowserProxy *br = nullptr);

      virtual ~TGo4RootBrowserProxy();

      Bool_t ProcessEvent(TGo4Slot *slot, TGo4Slot *source, Int_t id, void *param) override;

      void UnblockStatusOutput();

      void DrawPicture(const char *itemname, TGo4Picture *pic, TPad *pad);
      void DrawCondition(const char *itemname, TGo4Condition *con);
      void DrawItem(const char *itemname);

      void Message(const char *str1, const char *str2, Int_t blockdelay);

   protected:

      static Int_t fCanvasCounter;

      void UpdateRatemeter(TObject *obj);
      void UpdateLoginfo(TObject *obj);

      TCanvas *MakeCanvas(const char *title = nullptr);

      void SyncRootBrowserSlots();

      TGo4BrowserProxy *fBrowser{nullptr}; //!

      Bool_t  fLockMessage{kFALSE};  //!

   ClassDefOverride(TGo4RootBrowserProxy, 1);

};

#endif
