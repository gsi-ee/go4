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


void TGo4LogSettings::init()
{
   bool iswrite=TGo4Log::IsLogfileEnabled();

   IgnoreCombo->setCurrentItem(TGo4Log::GetIgnoreLevel());
   ModeCombo->setCurrentItem(TGo4Log::IsAutoEnabled());
   LogfilePrint->setChecked(TGo4Log::IsOutputEnabled());

   LogfileWrite->setChecked(iswrite);
   LogfileName->setText(TGo4Log::GetLogname());
   LogfileName->setEnabled(iswrite);
   FileNameLog->setEnabled(iswrite);
}

void TGo4LogSettings::LogfileDialog()
{
   QFileDialog fd( this, "Set GUI logfile:", TRUE );
   fd.setMode( QFileDialog::AnyFile );
   fd.setFilter( "Log textfile (*.log)" );
   if (fd.exec() != QDialog::Accepted) return;

   QString fileName = fd.selectedFile();
   if(!fileName.endsWith(".log")) fileName.append(".log");
   LogfileName->setText(fileName);
}
