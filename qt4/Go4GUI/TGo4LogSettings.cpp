#include "TGo4LogSettings.h"
#include <Q3FileDialog>
#include "TGo4Log.h"

TGo4LogSettings::TGo4LogSettings( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{ 
	//setObjectName(name);
	setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections 
			// from ui file to the setupUI
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
   Q3FileDialog fd( this, "Set GUI logfile:", TRUE );
   fd.setMode( Q3FileDialog::AnyFile );
   fd.setFilter( "Log textfile (*.log)" );
   if (fd.exec() != QDialog::Accepted) return;

   QString fileName = fd.selectedFile();
   if(!fileName.endsWith(".log")) fileName.append(".log");
   LogfileName->setText(fileName);
}
