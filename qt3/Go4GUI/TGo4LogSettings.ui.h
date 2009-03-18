
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
