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

#ifndef QFITNAMEDWIDGET_H
#define QFITNAMEDWIDGET_H

#include "QFitWidget.h"

class QGo4LineEdit;
class QLabel;

#ifdef WIN32
class QFitNamedWidget : public QFitWidget {
#else
#include <QtDesigner/QDesignerExportWidget>
class QDESIGNER_WIDGET_EXPORT QFitNamedWidget : public QFitWidget {
#endif

   Q_OBJECT

   public:
      QFitNamedWidget( QWidget* parent = 0, const char* name = 0 );
      ~QFitNamedWidget();

      virtual void FillSpecificData();

      QGo4LineEdit* NameEdt;
      QLabel* ClassNameLbl;
      QLabel* TextLabel2;
      QLabel* TextLabel1;
      QLabel* FullNameLbl;
      QGo4LineEdit* TitleEdt;

   public slots:
      virtual void ChangeName( const QString & name );
      virtual void NameEdt_textChanged( const QString & name );
      virtual void TitleEdt_textChanged( const QString & title );

};

#endif // QFITNAMEDWIDGET_H
