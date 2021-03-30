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
   QObject::connect(resize_b, SIGNAL(clicked()), this, SLOT(resizeall()));
   QObject::connect(SetScale, SIGNAL(clicked()), this, SLOT(SetScaleValues()));
   QObject::connect(AutoZoomBtn, SIGNAL(clicked()), this, SLOT(AutoZoomScales()));
   QObject::connect(UExpandX, SIGNAL(clicked()), this, SLOT(SlotUExpandX()));
   QObject::connect(ShiftL, SIGNAL(clicked()), this, SLOT(SlotShiftL()));
   QObject::connect(ShiftR, SIGNAL(clicked()), this, SLOT(SlotShiftR()));
   QObject::connect(ExpandX, SIGNAL(clicked()), this, SLOT(SlotExpandX()));
   QObject::connect(ShiftUp, SIGNAL(clicked()), this, SLOT(SlotShiftU()));
   QObject::connect(UExpandY, SIGNAL(clicked()), this, SLOT(SlotUExpandY()));
   QObject::connect(ExpandY, SIGNAL(clicked()), this, SLOT(SlotExpandY()));
   QObject::connect(ShiftDown, SIGNAL(clicked()), this, SLOT(SlotShiftD()));
   QObject::connect(ShiftUpZ, SIGNAL(clicked()), this, SLOT(SlotShiftZU()));
   QObject::connect(UExpandZ, SIGNAL(clicked()), this, SLOT(SlotUExpandZ()));
   QObject::connect(ExpandZ, SIGNAL(clicked()), this, SLOT(SlotExpandZ()));
   QObject::connect(ShiftDownZ, SIGNAL(clicked()), this, SLOT(SlotShiftZD()));

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
