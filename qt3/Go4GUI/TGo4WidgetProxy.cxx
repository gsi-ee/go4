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

#include "TGo4WidgetProxy.h"

#include "TPad.h"
#include "TGo4Slot.h"
#include "QGo4Widget.h"

Bool_t TGo4WidgetProxy::ProcessEvent(TGo4Slot* slot, TGo4Slot* source, Int_t id, void* param)
{
   if (fWidget==0) return kTRUE;

   TGo4Slot* src = (TGo4Slot*) source;

   if ((id==TGo4Slot::evObjAssigned) ||
       (id==TGo4Slot::evObjUpdated))
         fWidget->ProcessSignal(src->GetName(), true, src->GetAssignedObject(), src);
   else
   if (id==TGo4Slot::evDelete) {
      fWidget->ProcessSignal(src->GetName(), false, 0, src);
   }
   else
   if (id==TGo4Slot::evObjDeleted)
      fWidget->ProcessSignal(src->GetName(), false, (TObject*)param, src);
   else
   if (id==TGo4Slot::evSubslotUpdated)
      fWidget->ProcessSignal("TGo4Slot::evSubslotUpdated", true, (TObject*)param, src);

   // process further events which important for data management
   return (id==TGo4Slot::evDelete);
}

void TGo4WidgetProxy::ConnectPad(TPad* pad)
{
   if (pad!=0) {
      pad->Connect("RangeAxisChanged()","TGo4WidgetProxy",this,"PadRangeAxisChanged()");
      pad->Connect("Modified()","TGo4WidgetProxy",this,"PadModified()");
   }
}

void TGo4WidgetProxy::PadRangeAxisChanged()
{
   if (fWidget)
     fWidget->ProcessSignal("PadRangeAxisChanged", true, 0, 0);
}

void TGo4WidgetProxy::PadModified()
{
   if (fWidget)
     fWidget->ProcessSignal("PadModified", true, 0, 0);
}
