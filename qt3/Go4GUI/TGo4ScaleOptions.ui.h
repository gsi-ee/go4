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


void TGo4ScaleOptions::init()
{
}

void TGo4ScaleOptions::SlotShiftL()
{
   MoveScale(1, 0, 0);
}

void TGo4ScaleOptions::SlotShiftR()
{
    MoveScale(2, 0, 0);
}

void TGo4ScaleOptions::SlotShiftU()
{
    MoveScale(0, 2, 0);
}

void TGo4ScaleOptions::SlotShiftD()
{
    MoveScale(0, 1, 0);
}

void TGo4ScaleOptions::SlotShiftZU()
{
    MoveScale(0, 0, 2);
}

void TGo4ScaleOptions::SlotShiftZD()
{
    MoveScale(0, 0, 1);
}

void TGo4ScaleOptions::SlotExpandX()
{
   MoveScale(3, 0, 0);
}

void TGo4ScaleOptions::SlotExpandY()
{
   MoveScale(0, 3, 0);
}

void TGo4ScaleOptions::SlotExpandZ()
{
    MoveScale(0, 0, 3);
}

void TGo4ScaleOptions::SlotUExpandX()
{
    MoveScale(4, 0, 0);
}

void TGo4ScaleOptions::SlotUExpandY()
{
   MoveScale(0, 4, 0);
}

void TGo4ScaleOptions::SlotUExpandZ()
{
    MoveScale(0, 0, 4);
}

void TGo4ScaleOptions::resizeall()
{
   MoveScale(0, 0, 0);
}

void TGo4ScaleOptions::SetScaleValues()
{
   ServiceCall("ToggleScaleValues");
}

void TGo4ScaleOptions::SlotAutoZoom()
{
   MoveScale(5, 5, 5);
}

void TGo4ScaleOptions::MoveScale(int xaction, int yaction, int zaction)
{
   TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();
   if (panel!=0) panel->MoveScale(ExpandFactor->value(), xaction, yaction, zaction);
}
