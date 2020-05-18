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

#include "TGo4MarkerSettings.h"

TGo4MarkerSettings::TGo4MarkerSettings( QWidget* parent )
    : QDialog( parent )
{
   setObjectName("Go4MarkerSettings");
   setupUi(this);
    // regions:
   Bool_t LABELDRAW, LIMITSDRAW, INTDRAW,
          XMEANDRAW, YMEANDRAW, XRMSDRAW, YRMSDRAW,
          XMAXDRAW, YMAXDRAW, CMAXDRAW;
   TString NUMFORMAT;
   TGo4Condition::GetGlobalStyle(LABELDRAW, LIMITSDRAW, INTDRAW,
                                 XMEANDRAW, YMEANDRAW, XRMSDRAW, YRMSDRAW,
                                 XMAXDRAW, YMAXDRAW, CMAXDRAW, NUMFORMAT);
    DrawLabelButton->setChecked(LABELDRAW);
    DrawLimButton->setChecked(LIMITSDRAW);
    DrawIntButton->setChecked(INTDRAW);
    DrawXmeanButton->setChecked(XMEANDRAW);
    DrawYmeanButton->setChecked(YMEANDRAW);
    DrawXrmsButton->setChecked(XRMSDRAW);
    DrawYrmsButton->setChecked(YRMSDRAW);
    DrawXmaxButton->setChecked(XMAXDRAW);
    DrawYmaxButton->setChecked(YMAXDRAW);
    DrawCountsButton->setChecked(CMAXDRAW);
    WLabelFormatEdit->setText(NUMFORMAT.Data());

    // point markers:
    Bool_t HASLABEL, HASCONNECTOR, XDRAW, YDRAW, XBINDRAW, YBINDRAW, CONTDRAW;
    TGo4Marker::GetGlobalStyle(HASLABEL, HASCONNECTOR, XDRAW, YDRAW,
                               XBINDRAW, YBINDRAW, CONTDRAW, NUMFORMAT);

    DrawPLabelButton->setChecked(HASLABEL);
    DrawPConnectorButton->setChecked(HASCONNECTOR);
    DrawPXButton->setChecked(XDRAW);
    DrawPYButton->setChecked(YDRAW);
    DrawPXbinButton->setChecked(XBINDRAW);
    DrawPYbinButton->setChecked(YBINDRAW);
    DrawPCountsButton->setChecked(CONTDRAW);
    PLabelFormatEdit->setText(NUMFORMAT.Data());
}


void TGo4MarkerSettings::setFlags()
{
   TGo4Condition::SetGlobalStyle(DrawLabelButton->isChecked(),
                                 DrawLimButton->isChecked(),
                                 DrawIntButton->isChecked(),
                                 DrawXmeanButton->isChecked(),
                                 DrawYmeanButton->isChecked(),
                                 DrawXrmsButton->isChecked(),
                                 DrawYrmsButton->isChecked(),
                                 DrawXmaxButton->isChecked(),
                                 DrawYmaxButton->isChecked(),
                                 DrawCountsButton->isChecked(),
                                 WLabelFormatEdit->text().toLatin1().constData());

    TGo4Marker::SetGlobalStyle(DrawPLabelButton->isChecked(),
                               DrawPConnectorButton->isChecked(),
                               DrawPXButton->isChecked(),
                               DrawPYButton->isChecked(),
                               DrawPXbinButton->isChecked(),
                               DrawPYbinButton->isChecked(),
                               DrawPCountsButton->isChecked(),
                               PLabelFormatEdit->text().toLatin1().constData());

}
