#ifndef TGO4LOGSETTINGS_H
#define TGO4LOGSETTINGS_H

#include <QDialog>
#include "ui_TGo4LogSettings.h"

class TGo4LogSettings : public QDialog, public Ui::TGo4LogSettings
 {
     Q_OBJECT

 public:
     TGo4LogSettings( QWidget* parent = 0 );

 public slots:

	virtual void LogfileDialog();

 };

#endif
