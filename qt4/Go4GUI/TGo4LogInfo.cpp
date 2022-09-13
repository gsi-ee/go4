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

#include "TGo4LogInfo.h"

#include "TList.h"

#include <QFileDialog>
#include <QTextStream>
#include <QHeaderView>

TGo4LogInfo::TGo4LogInfo(QWidget *parent, const char *name) :
   QGo4Widget(parent, name)
{
   setupUi(this);

   setAcceptDrops(false);

   LogText->sortByColumn(1, Qt::DescendingOrder);

   LogText->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
   LogText->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
   LogText->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
   LogText->header()->setStretchLastSection(true);
}

void TGo4LogInfo::WorkWithInfo(TGo4Slot* slot)
{
    ResetWidget();
    AddLink(slot, "Loginfo");
}

void TGo4LogInfo::AddMessage(const QDateTime& dt, int level, QString msg)
{
   QString Date = dt.toString("dd.MM.yy  ");
   QString Time = dt.toString("hh.mm.ss  ");

   QTreeWidgetItem* Item = new QTreeWidgetItem();
   Item->setText(0, Date);
   Item->setText(1, Time);

   if (level == 0) {
      if(msg.contains("GO4-*")) level=1; else
      if(msg.contains("GO4-#")) level=2; else
      if(msg.contains("GO4-!")) level=3;
      msg.remove(0, 6);
   }

   switch(level) {
      case 2:
         Item->setText(2, "Warning");
         Item->setIcon(0, QIcon(":/icons/warn.png"));
         break;
      case 3:
         Item->setText(2, "Error");
         Item->setIcon(0, QIcon(":/icons/error.png"));
         break;
      default:
         Item->setText(2, "Info");
         Item->setIcon(0, QIcon(":/icons/info.png"));
         break;
   }

   Item->setText(3, msg);

   LogText->addTopLevelItem(Item);
   Item->setSelected(true);
   LogText->setCurrentItem(Item);
   LogText->scrollToItem(Item);
}

void TGo4LogInfo::linkedObjectUpdated(const char *linkname, TObject *linkobj)
{
   TList* lst = dynamic_cast<TList*>(linkobj);
   if (lst) {
      TListIter iter(lst, kFALSE);
      while (auto obj = iter()) {
         // first item is id of current status message, used to submit next request
         if (obj == lst->First()) continue;

         const char *msg = obj->GetName();

         const char *separ = strchr(msg,':');
         if (!separ || (strlen(separ) < 3)) continue;

         Long64_t tm = TString(msg, separ-msg).Atoll();

         QDateTime dt;
         dt.setMSecsSinceEpoch(tm*1000);

         separ++;
         int level = 1;
         if (*separ=='2') level=2; else
         if (*separ=='3') level=3;

         AddMessage(dt, level, separ+2);
      }
   } else if (linkobj) {
      AddMessage(QDateTime::currentDateTime(), 0, linkobj->GetName());
   }
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

    if (fd.exec() != QDialog::Accepted) return;

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
