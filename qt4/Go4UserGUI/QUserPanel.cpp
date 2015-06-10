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


#include "QUserPanel.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <QMimeData>

#include "Riostream.h"
#include "TClass.h"
#include "TCanvas.h"
#include "TGo4Proxy.h"
#include "QRootCanvas.h"

#include <sstream>

// this function called by Go4 GUI to start user panel
extern "C" Q_DECL_EXPORT void* StartUserPanel(void* parent)
{

   return new QUserPanel((QWidget*) parent);

//  Use this code to hide main go4 window and show only user gui

//  QWidget* w = (QWidget*) parent;
//  w->parentWidget()->hide();
//   return new QUserPanel(0);

}





// *********************************************************


/*
 *  Constructs a QUserPanel which is a child of 'parent', with the
 *  name 'name'.'
 */
QUserPanel::QUserPanel( QWidget* parent,  const char* name )
    : QGo4Widget( parent, name )
{
   setupUi(this);

   DragLbl->setText("");
   DragItemLbl->setText("");
   DragClassLbl->setText("");
   DragKindLbl->setText("");

   PrintLbl->setText("");

   QObject::connect(InfoBtn, SIGNAL(clicked()), this, SLOT(InfoBtn_clicked()));
   QObject::connect(EditBtn, SIGNAL(clicked()), this, SLOT(EditBtn_clicked()));
   QObject::connect(DrawBtn, SIGNAL(clicked()), this, SLOT(DrawBtn_clicked()));
   QObject::connect(fxDrawCanvas, SIGNAL(CanvasDropEvent(QDropEvent*,TPad*)), this, SLOT(CanvasDropEventSlot(QDropEvent*,TPad*)));
}

QUserPanel::~QUserPanel()
{
   fxDrawCanvas->getCanvas()->Clear();
}

QString QUserPanel::kindString(int kind)
{
   QString kndlabel;

   switch(kind) {
      case TGo4Access::kndNone: kndlabel = "unknown"; break;
      case TGo4Access::kndObject: kndlabel = "object"; break;
      case TGo4Access::kndFolder: kndlabel = "folder"; break;
      case TGo4Access::kndTreeBranch: kndlabel = "tree brunch"; break;
      case TGo4Access::kndTreeLeaf: kndlabel = "tree leaf"; break;
      case TGo4Access::kndGo4Param: kndlabel = "go4 parameter"; break;
      case TGo4Access::kndDataMember: kndlabel = "event data member"; break;
      case TGo4Access::kndEventElement: kndlabel = "event element"; break;
      default: kndlabel = "undefined"; break;
   }

   kndlabel = QString("Kind of draged item: ") + kndlabel +
              " (" + QString::number(kind) + ")";

   return kndlabel;
}

bool QUserPanel::IsAcceptDrag(const char* itemname, TClass* cl, int kind)
{
   bool res = false;

   switch (TabWidget->currentIndex()) {
      case 0:
        DragLbl->setText("Go4 GUI asks if widget accept dragged item");
        DragItemLbl->setText(itemname);
        DragClassLbl->setText(QString("Class: ") + (cl==0 ? "not exists (known)" : cl->GetName()));
        DragKindLbl->setText(kindString(kind));
        res = cl!=0;
        break;

     case 1:
        PrintLbl->setText(QString("Class: ") + (cl==0 ? "not exists (known)" : cl->GetName()));
        res = cl!=0;
        break;
   }
   // we will accept only items with known classes
   return res;
}

void QUserPanel::DropItem(const char* itemname, TClass* cl, int kind)
{
   switch (TabWidget->currentIndex()) {
      case 0:
         DragLbl->setText("User dropped item");
         DragItemLbl->setText(itemname);
         DragClassLbl->setText(cl==0 ? "No class specified" : cl->GetName());
         DragKindLbl->setText(kindString(kind));
         break;
      case 1:
         if (cl==0) {
            PrintLbl->setText("Can not drop item of uncknown class");
         } else {
            PrintLbl->setText(QString("Print item: ") + itemname);
            RemoveLink("PrintItem");
            AddLink(itemname, "PrintItem");
            PrintObject(GetLinked("PrintItem", 2));
         }

         break;
      case 2:
        DrawObjectOnCanvas(itemname);
        break;
   }
}

void QUserPanel::linkedObjectUpdated(const char* linkname, TObject* obj)
{
    if (strcmp(linkname, "PrintItem")==0)
      PrintObject(obj);
    else
    if (strcmp(linkname, "DrawItem")==0) {
       fxDrawCanvas->getCanvas()->Clear();
       fxDrawCanvas->getCanvas()->cd();
       obj->Draw("");
       fxDrawCanvas->getCanvas()->Update();
    }
}

void QUserPanel::linkedObjectRemoved(const char* linkname)
{
    if (strcmp(linkname, "PrintItem")==0)
      PrintObject(0);
    else
    if (strcmp(linkname, "DrawItem")==0) {
      RemoveLink("DrawItem");
      fxDrawCanvas->getCanvas()->Clear();
      fxDrawCanvas->getCanvas()->Update();
    }
}

void QUserPanel::InfoBtn_clicked()
{
   ShowItemInfo(DragItemLbl->text());
}

void QUserPanel::EditBtn_clicked()
{
   EditItem(DragItemLbl->text());
}

void QUserPanel::DrawBtn_clicked()
{
   DrawItem(DragItemLbl->text());
}

void QUserPanel::PrintObject(TObject* obj)
{
    PrintEdit->clear();
    if (obj==0) return;

    int out_pipe[2];
    int saved_stdout = dup(STDOUT_FILENO);  /* save stdout for display later */

    if( pipe(out_pipe) != 0 ) return;

    dup2(out_pipe[1], STDOUT_FILENO);   /* redirect stdout to the pipe */
    ::close(out_pipe[1]);

    obj->Print("");
    printf(" ");
    std::cout.flush();
    fflush(stdout);

    char sbuf[10000];
    memset(sbuf,0, sizeof(sbuf));
    read(out_pipe[0], sbuf, sizeof(sbuf)-1); /* read from pipe into buffer */

    dup2(saved_stdout, STDOUT_FILENO);  /* reconnect stdout for testing */
    ::close(out_pipe[0]);

    PrintEdit->setText(sbuf);
}

void QUserPanel::CanvasDropEventSlot(QDropEvent* event, TPad* pad)
{

   if (!event->mimeData()->hasText()) return;
   QString eventstr = event->mimeData()->text();
   event->acceptProposedAction();
   DrawObjectOnCanvas(eventstr.toLatin1().constData());
}

void QUserPanel::DrawObjectOnCanvas(const char* itemname)
{
   RemoveLink("DrawItem");
   AddLink(itemname, "DrawItem");
   TObject* obj = GetLinked("DrawItem", 2);
   if (obj!=0) {
      fxDrawCanvas->getCanvas()->Clear();
      fxDrawCanvas->getCanvas()->cd();
      obj->Draw("");
      fxDrawCanvas->getCanvas()->Update();
   }
}

