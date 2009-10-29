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

#ifndef TGO4OPENREMOTEDIALOG_H
#define TGO4OPENREMOTEDIALOG_H

#include <QDialog>
#include "ui_TGo4OpenRemoteDialog.h"

class TGo4OpenRemoteDialog : public QDialog, public Ui::TGo4OpenRemoteDialog
 {
     Q_OBJECT

 public:

    TGo4OpenRemoteDialog( QWidget* parent = 0 );

   virtual ~TGo4OpenRemoteDialog();

   const QString& GetFileName();

   const QString& GetFileType();

   const QString& GetServer();

 public slots:


   virtual void TypeSelectorBox_activated( const QString & tname);

   virtual void ServerEdit_textChanged( const QString &  txt);

   virtual void FileNameEdit_textChanged( const QString & txt);

 protected:
    QString fxServer;
    QString fxFile;
    QString fxType;


 };

#endif
