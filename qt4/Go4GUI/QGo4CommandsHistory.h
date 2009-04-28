#ifndef QGO4COMMANDSHISTORY_H
#define QGO4COMMANDSHISTORY_H

#include <QtDesigner/QDesignerExportWidget>
#include "qcombobox.h"

#include <QKeyEvent>

class QDESIGNER_WIDGET_EXPORT QGo4CommandsHistory : public QComboBox {
   Q_OBJECT

   public:
      QGo4CommandsHistory(QWidget* w, const char* name=0);
      virtual ~QGo4CommandsHistory();
      bool EnterPressed() const { return fiEnterPressed == 1; }
      void ResetEnterPressed() { fiEnterPressed = -1; }
      void SetEnterPressed(int val) { fiEnterPressed=val; }
   protected:
      virtual void keyPressEvent(QKeyEvent* e);
      int fiEnterPressed;
};

#endif
