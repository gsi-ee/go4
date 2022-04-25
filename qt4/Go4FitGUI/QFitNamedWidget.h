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

#ifndef QFITNAMEDWIDGET_H
#define QFITNAMEDWIDGET_H

#include "QFitWidget.h"

class QGo4LineEdit;
class QLabel;

#ifdef __GO4DESIGNER__
#include <QtDesigner/QDesignerExportWidget>
class QDESIGNER_WIDGET_EXPORT QFitNamedWidget : public QFitWidget {
#else
class QFitNamedWidget : public QFitWidget {
#endif

   Q_OBJECT

   public:
      QFitNamedWidget(QWidget *parent = nullptr, const char *name = nullptr);
      ~QFitNamedWidget();

      void FillSpecificData() override;

      QGo4LineEdit* NameEdt{nullptr};
      QLabel* ClassNameLbl{nullptr};
      QLabel* TextLabel2{nullptr};
      QLabel* TextLabel1{nullptr};
      QLabel* FullNameLbl{nullptr};
      QGo4LineEdit* TitleEdt{nullptr};

   public slots:
      virtual void ChangeName( const QString & name );
      virtual void NameEdt_textChanged( const QString & name );
      virtual void TitleEdt_textChanged( const QString & title );

};

#endif
