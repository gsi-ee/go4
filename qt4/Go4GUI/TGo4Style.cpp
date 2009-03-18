#include "TGo4Style.h"
#include "TColor.h"
#include <QColorDialog>
#include "TGo4WorkSpace.h"
#include "TGo4ViewPanel.h"

TGo4Style::TGo4Style( QWidget* parent, const char* name, Qt::WFlags fl )
    : QWidget( parent, name, fl )
{ 
	//setObjectName(name);
	setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI

}


void TGo4Style::SetPadColor()
{
   QColor c = QColorDialog::getColor();
   if (!c.isValid()) return;

   Int_t color = TColor::GetColor(c.red(), c.green(), c.blue());

   TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();
   if (panel!=0)
      panel->ChangeDrawOption(101, color, 0);
}

void TGo4Style::SetPalette( int t )
{
   TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();
   if (panel!=0)
      panel->ChangeDrawOption(100, t, 0);
}
