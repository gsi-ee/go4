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

#ifndef TGO4WIDGETPROXY_H
#define TGO4WIDGETPROXY_H

#include "TGo4WidgetProxyBase.h"

class QGo4Widget;

class TGo4WidgetProxy : public TGo4WidgetProxyBase {
   public:

      TGo4WidgetProxy() : TGo4WidgetProxyBase(), fWidget(nullptr) {}
      TGo4WidgetProxy(QGo4Widget* w) : TGo4WidgetProxyBase(), fWidget(w) {}

      QGo4Widget*  GetWidget() const { return fWidget; }
      Bool_t Use() const override { return kFALSE; }
      Bool_t ProcessEvent(TGo4Slot *slot, TGo4Slot *source, Int_t id, void *param) override;
      void PadRangeAxisChanged() override;
      void PadModified() override;

   protected:

      QGo4Widget*  fWidget{nullptr};   //!
};

#endif
