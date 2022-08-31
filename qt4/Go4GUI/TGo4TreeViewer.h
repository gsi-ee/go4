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

#ifndef TGO4TREEVIEWER_H
#define TGO4TREEVIEWER_H

#include "QGo4Widget.h"
#include "ui_TGo4TreeViewer.h"

class TObject;

class TGo4TreeViewer : public QGo4Widget, public Ui::TGo4TreeViewer
{
   Q_OBJECT

public:

   TGo4TreeViewer(QWidget *parent = nullptr, const char* name = nullptr);

   void ResetWidget() override;
   virtual void WorkWithLeaf(const char *itemname);
   virtual void ProcessDropEvent(QGo4LineEdit* edt, bool caninit);
   void requestedObjectCreated(const char *itemname, TClass* cl) override;

public slots:

   virtual void NewHistBtn_clicked();
   virtual void TreeClearBtn_clicked();
   virtual void TreeDrawBtn_clicked();
   virtual void XFieldEdt_dropped();
   virtual void YFieldEdt_dropped();
   virtual void ZFieldEdt_dropped();
   virtual void cutEdit_dropped();

protected:

   QString fxTreeName;

 };

#endif
