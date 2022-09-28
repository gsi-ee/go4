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

#include "TGo4OpenRemoteDialog.h"
#include "TGo4QSettings.h"

TGo4OpenRemoteDialog::TGo4OpenRemoteDialog( QWidget *parent)
   : QDialog( parent )
{
   setObjectName("Go4OpenRemoteDialog");
   setupUi(this);
   QObject::connect(TypeSelectorBox, QOverload<int>::of(&QComboBox::activated), this, &TGo4OpenRemoteDialog::TypeSelectorBox_activated);
   QObject::connect(ServerEdit, &QLineEdit::textChanged, this, &TGo4OpenRemoteDialog::ServerEdit_textChanged);
   QObject::connect(FileNameEdit, &QLineEdit::textChanged, this, &TGo4OpenRemoteDialog::FileNameEdit_textChanged);

   QString hostname, filename, protocol;
   go4sett->getRemoteFileSett(hostname, filename, protocol);

   ServerEdit->setText(hostname);
   fxServer = hostname;
   FileNameEdit->setText(filename);
   fxFile = filename;
   fxType = protocol;
   int indx = 3;
   if(protocol.contains("root:"))
      indx = 0;
   else if (protocol.contains("rfio:"))
      indx = 1;
   else if (protocol.contains("http:"))
      indx = 2;

   if (indx < 3) {
      TypeSelectorBox->setCurrentIndex(indx);
      TypeSelectorBox_activated(indx);
   } else {
      TypeSelectorBox->setCurrentText(protocol);
   }
}

TGo4OpenRemoteDialog::~TGo4OpenRemoteDialog()
{
   QString server = GetServer();
   QString fname = GetFileName();
   QString protocol = GetFileType();
   if(protocol.contains("root:"))
       protocol = "root:";
   else if (protocol.contains("rfio:"))
       protocol = "rfio:";
   else if (protocol.contains("http:"))
       protocol = "http:";
   else
       protocol = "local:";
   go4sett->setRemoteFileSett(server, fname, protocol);
}

void TGo4OpenRemoteDialog::TypeSelectorBox_activated(int indx)
{
   QString tname = TypeSelectorBox->itemText(indx);
   if(tname.contains("root:") || tname.contains("http:") || tname.contains("rfio:"))
      ServerEdit->setEnabled(true);
   else
      ServerEdit->setEnabled(false);
   fxType = tname;
}

const QString& TGo4OpenRemoteDialog::GetFileName()
{
   return fxFile;
}

const QString& TGo4OpenRemoteDialog::GetFileType()
{
   return fxType;
}


const QString& TGo4OpenRemoteDialog::GetServer()
{
   return fxServer;
}

void TGo4OpenRemoteDialog::ServerEdit_textChanged( const QString &  txt)
{
   fxServer=txt;
}


void TGo4OpenRemoteDialog::FileNameEdit_textChanged( const QString & txt)
{
   fxFile=txt;
}
