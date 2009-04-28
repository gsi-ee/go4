#include "TGo4ScaleOptions.h"
#include <QLineEdit>
#include "Riostream.h"
#include "TGo4WorkSpace.h"
#include "TGo4ViewPanel.h"
#include "TGo4SetScaleValues.h"

TGo4ScaleOptions::TGo4ScaleOptions(QWidget *parent, const char* name)
         : QGo4Widget(parent,name)
{
	setupUi(this);
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

void TGo4ScaleOptions::MoveScale(int xaction, int yaction, int zaction)
{
   TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();
   if (panel!=0) panel->MoveScale(ExpandFactor->value(), xaction, yaction, zaction);
}
