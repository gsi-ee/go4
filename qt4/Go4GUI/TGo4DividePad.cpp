#include "TGo4DividePad.h"

#include "TGo4WorkSpace.h"
#include "TGo4ViewPanel.h"

TGo4DividePad::TGo4DividePad( QWidget* parent, const char* name, Qt::WFlags fl )
    : QWidget( parent, name, fl )
{ 
	setObjectName(name); 
	setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
}

void TGo4DividePad::DivideCanvas()
{
  TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();

  if (panel!=0)
     panel->Divide(SpinBoxX->value(), SpinBoxY->value());
}
