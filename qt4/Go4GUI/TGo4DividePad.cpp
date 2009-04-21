#include "TGo4DividePad.h"

#include "TGo4WorkSpace.h"
#include "TGo4ViewPanel.h"

TGo4DividePad::TGo4DividePad( QWidget* parent, const char* name, Qt::WFlags fl )
    : QWidget( parent, name, fl )
{
	setObjectName(name);
	setupUi(this);
}

void TGo4DividePad::DivideCanvas()
{
  TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();

  if (panel!=0)
     panel->Divide(SpinBoxX->value(), SpinBoxY->value());
}
