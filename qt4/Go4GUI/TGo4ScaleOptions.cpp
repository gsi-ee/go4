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

#include "TGo4ScaleOptions.h"

#include "TGo4MdiArea.h"
#include "TGo4ViewPanel.h"

TGo4ScaleOptions::TGo4ScaleOptions(QWidget *parent, const char* name)
         : QGo4Widget(parent,name)
{
   setupUi(this);
   QObject::connect(resize_b, &QPushButton::clicked, this, &TGo4ScaleOptions::resizeall);
   QObject::connect(SetScale, &QPushButton::clicked, this, &TGo4ScaleOptions::SetScaleValues);
   QObject::connect(AutoZoomBtn, &QPushButton::clicked, this, &TGo4ScaleOptions::AutoZoomScales);
   QObject::connect(UExpandX, &QPushButton::clicked, this, &TGo4ScaleOptions::SlotUExpandX);
   QObject::connect(ShiftL, &QPushButton::clicked, this, &TGo4ScaleOptions::SlotShiftL);
   QObject::connect(ShiftR, &QPushButton::clicked, this, &TGo4ScaleOptions::SlotShiftR);
   QObject::connect(ExpandX, &QPushButton::clicked, this, &TGo4ScaleOptions::SlotExpandX);
   QObject::connect(ShiftUp, &QPushButton::clicked, this, &TGo4ScaleOptions::SlotShiftU);
   QObject::connect(UExpandY, &QPushButton::clicked, this, &TGo4ScaleOptions::SlotUExpandY);
   QObject::connect(ExpandY, &QPushButton::clicked, this, &TGo4ScaleOptions::SlotExpandY);
   QObject::connect(ShiftDown, &QPushButton::clicked, this, &TGo4ScaleOptions::SlotShiftD);
   QObject::connect(ShiftUpZ, &QPushButton::clicked, this, &TGo4ScaleOptions::SlotShiftZU);
   QObject::connect(UExpandZ, &QPushButton::clicked, this, &TGo4ScaleOptions::SlotUExpandZ);
   QObject::connect(ExpandZ, &QPushButton::clicked, this, &TGo4ScaleOptions::SlotExpandZ);
   QObject::connect(ShiftDownZ, &QPushButton::clicked, this, &TGo4ScaleOptions::SlotShiftZD);
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

void TGo4ScaleOptions::AutoZoomScales()
{
   MoveScale(5, 5, 5);
}

void TGo4ScaleOptions::MoveScale(int xaction, int yaction, int zaction)
{
   TGo4ViewPanel* panel = TGo4MdiArea::Instance()->GetActivePanel();
   if (panel) panel->MoveScale(ExpandFactor->value(), xaction, yaction, zaction);
}
