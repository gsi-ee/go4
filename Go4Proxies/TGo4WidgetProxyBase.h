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

#ifndef TGO4WIDGETPROXYBASE_H
#define TGO4WIDGETPROXYBASE_H

#include "TGo4Proxy.h"

class TPad;

class TGo4WidgetProxyBase : public TGo4Proxy {
   public:
      TGo4WidgetProxyBase() : TGo4Proxy() {}

      virtual ~TGo4WidgetProxyBase() {}

      Bool_t Use() const override { return kFALSE; }

      void ConnectPad(TPad *pad);

      virtual void PadRangeAxisChanged() {}

      virtual void PadModified() {}

   ClassDefOverride(TGo4WidgetProxyBase, 1);
};

#endif
