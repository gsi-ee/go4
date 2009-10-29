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

#ifndef QFITMODELWIDGET_H
#define QFITMODELWIDGET_H

#include "QFitNamedWidget.h"

class QLabel;
class QSpinBox;
class QCheckBox;

#ifdef WIN32
class QFitModelWidget : public QFitNamedWidget {
#else
#include <QtDesigner/QDesignerExportWidget>
class QDESIGNER_WIDGET_EXPORT QFitModelWidget : public QFitNamedWidget {
#endif

  Q_OBJECT

  public:
      QFitModelWidget( QWidget* parent = 0, const char* name = 0 );
      ~QFitModelWidget();

      QCheckBox* AmplChk;
      QLabel* textLabel1;
      QCheckBox* BuffersChk;
      QSpinBox* GroupSpin;

   public slots:
      virtual void AmplChk_toggled( bool chk );
      virtual void BuffersChk_toggled( bool chk );
      virtual void GroupSpin_valueChanged( int );
};

#endif // QFITMODELWIDGET_H
