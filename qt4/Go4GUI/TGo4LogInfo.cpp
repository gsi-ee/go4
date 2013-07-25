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

#include "TGo4LogInfo.h"

#include "TObject.h"
#include <QFileDialog>
#include <QDateTime>
#include <QTextStream>
#include <QHeaderView>


TGo4LogInfo::TGo4LogInfo(QWidget *parent, const char* name)
         : QGo4Widget(parent, name)
{
   setupUi(this);

   setAcceptDrops(FALSE);

   LogText->sortByColumn(1, Qt::DescendingOrder);

   LogText->header()->setResizeMode(0, QHeaderView::ResizeToContents);
   LogText->header()->setResizeMode(1, QHeaderView::ResizeToContents);
   LogText->header()->setResizeMode(2, QHeaderView::ResizeToContents);

   LogText->header()->setStretchLastSection(true);
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

   QTreeWidgetItem* Item = new QTreeWidgetItem();
   Item->setText(0, Date);
   Item->setText(1, Time);

   if(Name.contains("GO4-*")) {
      Item->setText(2, "Info");
      Item->setIcon(0, QIcon(":/icons/info.png"));
   } else
   if (Name.contains("GO4-!")) {
      Item->setText(2, "Error");
      Item->setIcon(0, QIcon(":/icons/error.png"));
   } else
   if(Name.contains("GO4-#")) {
      Item->setText(2, "Warning");
      Item->setIcon(0, QIcon(":/icons/warn.png"));
   }

   Name.remove(0, 6);
   Item->setText(3, Name);

   LogText->addTopLevelItem(Item);
   Item->setSelected(true);
   LogText->setCurrentItem(Item);
   LogText->scrollToItem(Item);
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
    fd.setFileMode( QFileDialog::AnyFile );
    fd.setAcceptMode(QFileDialog::AcceptSave);

    if ( fd.exec() != QDialog::Accepted ) return;

    QStringList flst = fd.selectedFiles();
    if (flst.isEmpty()) return;

    QString fileName = flst[0];
    if(!fileName.endsWith(".txt")) fileName.append(".txt");
    QFile NewFile(fileName);
    NewFile.open( QIODevice::ReadWrite | QIODevice::Append );
    QTextStream t( &NewFile );

    QTreeWidgetItemIterator it(LogText);
    while (*it) {
       QTreeWidgetItem* itm = *it++;
       t << itm->text(0) << " " << itm->text(1) << " " << itm->text(3) << "\n";
    }
    NewFile.close();
}
