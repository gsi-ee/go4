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

#include "TGo4ServerProxy.h"

#include "TGo4Slot.h"

TGo4ServerProxy::TGo4ServerProxy() :
   TGo4Proxy(),
   fxParentSlot(0),
   fbAnalysisSettingsReady(kFALSE),
   fNodeName(),
   fInfoStr()
{
}

TGo4ServerProxy::~TGo4ServerProxy()
{
}

TGo4Slot* TGo4ServerProxy::SettingsSlot()
{
   return fxParentSlot==0 ? 0 : fxParentSlot->FindChild("Settings");
}

TGo4Slot* TGo4ServerProxy::RatemeterSlot()
{
   return fxParentSlot==0 ? 0 : fxParentSlot->FindChild("Ratemeter");
}

TGo4Slot* TGo4ServerProxy::LoginfoSlot()
{
   return fxParentSlot==0 ? 0 : fxParentSlot->FindChild("Loginfo");
}

TGo4Slot* TGo4ServerProxy::DebugOutputSlot()
{
   return fxParentSlot==0 ? 0 : fxParentSlot->FindChild("Debugoutput");
}

const char* TGo4ServerProxy::GetContainedObjectInfo()
{
   fInfoStr = "";
   if (!IsConnected()) fInfoStr = "Not connected"; else
   if (IsViewer()) fInfoStr = "Observer"; else
   if (IsController()) fInfoStr = "Controller"; else
   if (IsAdministrator()) fInfoStr = "Administrator";
   return fInfoStr.Data();
}

