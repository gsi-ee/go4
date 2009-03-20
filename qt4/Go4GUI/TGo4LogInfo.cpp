#include "TGo4LogInfo.h"

#include "TObject.h"
#include <QFileDialog>
#include <QDateTime>
#include <Q3ListView>
#include <QTextStream>


TGo4LogInfo::TGo4LogInfo(QWidget *parent, const char* name)
         : QGo4Widget(parent, name)
{
	setupUi(this);
			// put slot connections here!
			// note: Qt4 uic will add all existing connections
			// from ui file to the setupUI
   setAcceptDrops(FALSE);
   infoIcon = QPixmap(":/icons/info.png");
   errorIcon = QPixmap(":/icons/error.png");
   warnIcon = QPixmap(":/icons/warn.png");

   LogText->setSorting(0, FALSE);
}

void TGo4LogInfo::WorkWithInfo(TGo4Slot* slot)
{
    ResetWidget();
    AddLink(slot, "Loginfo");
}

void TGo4LogInfo::linkedObjectUpdated( const char * linkname, TObject * obj )
{
   if (obj==0) return;

   QDateTime dt = QDateTime::currentDateTime();
   QString Name = obj->GetName();
   QString Date = dt.toString("dd.MM.yy  ");
   QString Time = dt.toString("hh.mm.ss  ");

   Q3ListViewItem *Item = new Q3ListViewItem(LogText, Date, Time);
   if(Name.contains("GO4-*")) {
      Item->setText(3, "Info");
      Item->setPixmap(0, infoIcon);
    } else if (Name.contains("GO4-!")) {
      Item->setText(3, "Error");
      Item->setPixmap(0, errorIcon);
    } else if(Name.contains("GO4-#")) {
      Item->setText(3, "Warning");
      Item->setPixmap(0, warnIcon);
      }
    Name.remove(0, 6);
    Item->setText(2, Name);
    LogText->setSelected(Item, true);
}


void TGo4LogInfo::ClearLogInfo()
{
   LogText->clear();
}

void TGo4LogInfo::SaveLogInfo()
{
    QString TextToSave;
    QFileDialog fd( this, "Save analysis log window", QString(),
          "Plain text (*.txt)");
    fd.setMode( QFileDialog::AnyFile );

    if ( fd.exec() != QDialog::Accepted ) return;

    QString fileName = fd.selectedFile();
    if(!fileName.endsWith(".txt")) fileName.append(".txt");
    QFile NewFile(fileName);
    NewFile.open( IO_ReadWrite | IO_Append );
    QTextStream t( &NewFile );

    Q3ListViewItemIterator it(LogText);
      // iterate through all items of the list view
    for ( ; it.current(); ++it ) {
       Q3ListViewItem* itm = it.current();
       t << itm->text(0) << " " << itm->text(1) << " " << itm->text(2) << endl;
    }
    NewFile.close();
}
