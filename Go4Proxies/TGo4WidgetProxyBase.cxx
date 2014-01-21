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

#include "TGo4WidgetProxyBase.h"

#include "TPad.h"

void TGo4WidgetProxyBase::ConnectPad(TPad* pad)
{
   if (pad!=0) {
      pad->Connect("RangeAxisChanged()","TGo4WidgetProxyBase",this,"PadRangeAxisChanged()");
      pad->Connect("Modified()","TGo4WidgetProxyBase",this,"PadModified()");
   }
}
