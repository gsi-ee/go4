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

#ifndef TGO4PRINTWIDGET_H
#define TGO4PRINTWIDGET_H

#include <QDialog>
#include "ui_TGo4PrintWidget.h"

class TGo4PrintWidget : public QDialog, public Ui::TGo4PrintWidget
 {
     Q_OBJECT

 public:
    TGo4PrintWidget( QWidget* parent = 0 );

 public slots:

   QString GetPrinter();

   QString GetPrintProg();


 };

#endif
