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

#ifndef TGO4MACRODIALOG_H
#define TGO4MACRODIALOG_H

#include <QDialog>
#include "ui_TGo4MacroDialog.h"

class TGo4MacroDialog : public QDialog, public Ui::TGo4MacroDialog
 {
     Q_OBJECT

 public:

    TGo4MacroDialog( QWidget* parent = 0 );

   const QString& getCommand();

 public slots:



   virtual void setCommand( const QString & selection );

 protected:
    QString fxCommand;


};

#endif

