// $Id: TGo4UserCommandsDialog.h 489 2009-11-04 12:20:26Z linev $
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

#ifndef TGO4USERCOMMANDSDIALOG_H
#define TGO4USERCOMMANDSDIALOG_H

#include <QDialog>
#include "ui_TGo4UserCommandsDialog.h"

class TGo4UserCommandsDialog : public QDialog, public Ui::TGo4UserCommandsDialog
 {
     Q_OBJECT

 public:

    TGo4UserCommandsDialog( QWidget* parent = 0 );


    /** read command for index id*/
    QString GetCommand(int id);

    /** tooltip for index id  */
    QString GetTooltip(int id);

 public slots:


 virtual void ButtonPressed(QAbstractButton*);

 protected:


};

#endif

