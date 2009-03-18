#ifndef QFITMODELWIDGET_H
#define QFITMODELWIDGET_H

#include "QFitNamedWidget.h"

class QLabel;
class QSpinBox;
class QCheckBox;

class QT_WIDGET_PLUGIN_EXPORT QFitModelWidget : public QFitNamedWidget {
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
