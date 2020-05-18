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
    DrawLabelButton->setChecked(TGo4Condition::fgbLABELDRAW);
    DrawIntButton->setChecked(TGo4Condition::fgbINTDRAW);
    DrawLimButton->setChecked(TGo4Condition::fgbLIMITSDRAW);
    DrawCountsButton->setChecked(TGo4Condition::fgbCMAXDRAW);
    DrawYrmsButton->setChecked(TGo4Condition::fgbYRMSDRAW);
    DrawXrmsButton->setChecked(TGo4Condition::fgbXRMSDRAW);
    DrawXmaxButton->setChecked(TGo4Condition::fgbXMAXDRAW);
    DrawYmaxButton->setChecked(TGo4Condition::fgbYMAXDRAW);
    DrawXmeanButton->setChecked(TGo4Condition::fgbXMEANDRAW);
    DrawYmeanButton->setChecked(TGo4Condition::fgbYMEANDRAW);

    WLabelFormatEdit->setText(TGo4Condition::fgxNUMFORMAT.Data());

    // point markers:
    Bool_t HASLABEL, HASCONNECTOR, XDRAW, YDRAW, XBINDRAW, YBINDRAW, CONTDRAW;
    TString NUMFORMAT;
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
    TGo4Condition::fgbLABELDRAW=DrawLabelButton->isChecked();
    TGo4Condition::fgbINTDRAW=DrawIntButton->isChecked();
    TGo4Condition::fgbLIMITSDRAW=DrawLimButton->isChecked();
    TGo4Condition::fgbCMAXDRAW=DrawCountsButton->isChecked();
    TGo4Condition::fgbYRMSDRAW=DrawYrmsButton->isChecked();
    TGo4Condition::fgbXRMSDRAW=DrawXrmsButton->isChecked();
    TGo4Condition::fgbXMAXDRAW=DrawXmaxButton->isChecked();
    TGo4Condition::fgbYMAXDRAW=DrawYmaxButton->isChecked();
    TGo4Condition::fgbXMEANDRAW=DrawXmeanButton->isChecked();
    TGo4Condition::fgbYMEANDRAW=DrawYmeanButton->isChecked();
    TGo4Condition::fgxNUMFORMAT = WLabelFormatEdit->text().toLatin1().constData();

    TGo4Marker::SetGlobalStyle(DrawPLabelButton->isChecked(),
                               DrawPConnectorButton->isChecked(),
                               DrawPXButton->isChecked(),
                               DrawPYButton->isChecked(),
                               DrawPXbinButton->isChecked(),
                               DrawPYbinButton->isChecked(),
                               DrawPCountsButton->isChecked(),
                               PLabelFormatEdit->text().toLatin1().constData());

}
