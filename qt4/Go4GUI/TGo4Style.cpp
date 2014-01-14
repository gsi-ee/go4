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

#include "TGo4Style.h"
#include "TColor.h"
#include <QColorDialog>
#include "TGo4MdiArea.h"
#include "TGo4ViewPanel.h"

TGo4Style::TGo4Style( QWidget* parent, const char* name, Qt::WFlags fl ) :
   QWidget( parent, fl )
{
   setObjectName( name ? name : "Go4Style");
   setupUi(this);
}


void TGo4Style::SetPadColor()
{
   QColor c = QColorDialog::getColor();
   if (!c.isValid()) return;

   Int_t color = TColor::GetColor(c.red(), c.green(), c.blue());

   TGo4ViewPanel* panel = TGo4MdiArea::Instance()->GetActivePanel();
   if (panel!=0)
      panel->ChangeDrawOption(101, color, 0);
}

void TGo4Style::SetPalette( int t )
{
   TGo4ViewPanel* panel = TGo4MdiArea::Instance()->GetActivePanel();
   if (panel!=0)
      panel->ChangeDrawOption(100, t, 0);
}
