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

/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/


void TGo4MarkerSettings::init()
{
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

    // point markers:
    DrawPLabelButton->setChecked(TGo4Marker::fgbHASLABEL);
    DrawPConnectorButton->setChecked(TGo4Marker::fgbHASCONNECTOR);
    DrawPXButton->setChecked(TGo4Marker::fgbXDRAW);
    DrawPYButton->setChecked(TGo4Marker::fgbYDRAW);
    DrawPXbinButton->setChecked(TGo4Marker::fgbXBINDRAW);
    DrawPYbinButton->setChecked(TGo4Marker::fgbYBINDRAW);
    DrawPCountsButton->setChecked(TGo4Marker::fgbCONTDRAW);





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

    TGo4Marker::fgbHASLABEL=DrawPLabelButton->isChecked();
    TGo4Marker::fgbHASCONNECTOR=DrawPConnectorButton->isChecked();
    TGo4Marker::fgbXDRAW=DrawPXButton->isChecked();
    TGo4Marker::fgbYDRAW=DrawPYButton->isChecked();
    TGo4Marker::fgbXBINDRAW=DrawPXbinButton->isChecked();
    TGo4Marker::fgbYBINDRAW=DrawPYbinButton->isChecked();
    TGo4Marker::fgbCONTDRAW=DrawPCountsButton->isChecked();
}
