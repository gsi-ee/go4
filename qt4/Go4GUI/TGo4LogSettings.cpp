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
