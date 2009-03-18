/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void TGo4OpenRemoteDialog::init()
{
   QString hostname, filename, protocol;
   go4sett->getRemoteFileSett(hostname, filename, protocol);

   ServerEdit->setText(hostname);
   fxServer=hostname;
   FileNameEdit->setText(filename);
   fxFile=filename;
   fxType=protocol;
   if(protocol.contains("root:"))
      TypeSelectorBox->setCurrentItem(0);
   else
   if (protocol.contains("rfio:"))
      TypeSelectorBox->setCurrentItem(1);
   else
   if (protocol.contains("http:"))
      TypeSelectorBox->setCurrentItem(2);
   else
      TypeSelectorBox->setCurrentItem(3);
   TypeSelectorBox_activated(protocol);
}

void TGo4OpenRemoteDialog::destroy()
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
