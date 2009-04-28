#include "TGo4OpenRemoteDialog.h"
#include "TGo4QSettings.h"

TGo4OpenRemoteDialog::TGo4OpenRemoteDialog( QWidget* parent)
			: QDialog( parent )

{
	setObjectName("Go4OpenRemoteDialog");
	setupUi(this);
   QString hostname, filename, protocol;
   go4sett->getRemoteFileSett(hostname, filename, protocol);

   ServerEdit->setText(hostname);
   fxServer=hostname;
   FileNameEdit->setText(filename);
   fxFile=filename;
   fxType=protocol;
   if(protocol.contains("root:"))
      TypeSelectorBox->setCurrentIndex(0);
   else
   if (protocol.contains("rfio:"))
      TypeSelectorBox->setCurrentIndex(1);
   else
   if (protocol.contains("http:"))
      TypeSelectorBox->setCurrentIndex(2);
   else
      TypeSelectorBox->setCurrentIndex(3);
   TypeSelectorBox_activated(protocol);
}

TGo4OpenRemoteDialog::~TGo4OpenRemoteDialog()
{
   QString server = GetServer();
   QString fname = GetFileName();
   QString protocol = GetFileType();
   if(protocol.contains("root:"))
       protocol="root:";
   else
   if (protocol.contains("rfio:"))
       protocol="rfio:";
   else
   if (protocol.contains("http:"))
       protocol="http:";
   else
       protocol="local:";
   go4sett->setRemoteFileSett(server, fname, protocol);
}



void TGo4OpenRemoteDialog::TypeSelectorBox_activated( const QString & tname)
{
   if(tname.contains("root:") || tname.contains("http:") || tname.contains("rfio:"))
      ServerEdit->setEnabled(true);
   else
      ServerEdit->setEnabled(false);
   fxType=tname;
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
