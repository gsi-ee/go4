#ifndef TGO4HSERVERCPNNECTION_H
#define TGO4HSERVERCPNNECTION_H

#include <QDialog>
#include "ui_TGo4HServerConnection.h"

class TGo4HServerConnection : public QDialog, public Ui::TGo4HServerConnection
 {
     Q_OBJECT

 public:
     TGo4HServerConnection( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 )
                        : QDialog( parent, fl )
        {
           setObjectName(name ? name : "Go4HServConnection");
           setModal(modal);

          setupUi(this);
        }
 };

#endif
