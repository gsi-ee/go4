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

#include "TGo4LogSettings.h"
#include <QFileDialog>
#include "TGo4Log.h"

TGo4LogSettings::TGo4LogSettings( QWidget* parent )
    : QDialog( parent )
{
   setObjectName("Go4LogSettings");
   setupUi(this);
   bool iswrite=TGo4Log::IsLogfileEnabled();

   IgnoreCombo->setCurrentIndex(TGo4Log::GetIgnoreLevel());
   ModeCombo->setCurrentIndex(TGo4Log::IsAutoEnabled());
   LogfilePrint->setChecked(TGo4Log::IsOutputEnabled());

   LogfileWrite->setChecked(iswrite);
   LogfileName->setText(TGo4Log::GetLogname());
   LogfileName->setEnabled(iswrite);
   FileNameLog->setEnabled(iswrite);
}

void TGo4LogSettings::LogfileDialog()
{
   QFileDialog fd( this, "Set GUI logfile", "", "Log textfile (*.log)");
   fd.setFileMode( QFileDialog::AnyFile );
   if (fd.exec() != QDialog::Accepted) return;

   QStringList flst = fd.selectedFiles();
   if (flst.isEmpty()) return;

   QString fileName = flst[0];
   if(!fileName.endsWith(".log")) fileName.append(".log");
   LogfileName->setText(fileName);
}
