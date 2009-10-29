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

#ifndef TGO4WIDGETPROXY_H
#define TGO4WIDGETPROXY_H

#include "TGo4Proxy.h"

class QGo4Widget;
class TPad;

class TGo4WidgetProxy : public TGo4Proxy {
   public:

      TGo4WidgetProxy() : TGo4Proxy(), fWidget(0) {}

      TGo4WidgetProxy(QGo4Widget* w) : TGo4Proxy(), fWidget(w) {}

      virtual ~TGo4WidgetProxy() {}

      QGo4Widget*  GetWidget() const { return fWidget; }

      virtual Bool_t Use() const { return kFALSE; }

      virtual Bool_t ProcessEvent(TGo4Slot* slot, TGo4Slot* source, Int_t id, void* param);

      void ConnectPad(TPad* pad);

      void PadRangeAxisChanged();

      void PadModified();

   protected:

      QGo4Widget*  fWidget;   //!

   ClassDef(TGo4WidgetProxy, 1);
};

#endif
