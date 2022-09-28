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

#ifndef ExampleWidget_h
#define ExampleWidget_h

#include <QWidget>
#include "ui_ExampleWidget.h"

class QRootCanvas;
class TH1F;

class ExampleWidget : public QWidget, public Ui::ExampleWidget
{
   Q_OBJECT

   public:

      ExampleWidget(QWidget *parent = nullptr, const char *name = nullptr);

      virtual ~ExampleWidget();

   protected:
      void resizeEvent(QResizeEvent * e) override;

      TH1F *fHisto{nullptr};

   public slots:

      void InfoButton_clicked();
      void EditorButton_clicked();
      void ExitButton_clicked();

};

#endif
