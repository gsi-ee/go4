#ifndef QGO4LINEEDIT_H
#define QGO4LINEEDIT_H

#include "qlineedit.h"
#include <QtDesigner/QDesignerExportWidget>

#include <QDropEvent>
#include <QDragEnterEvent>

/**
 * @author Mohammad Al-Turany
 * @author S.Linev
 * @since 14.11.2001
 */

class QDESIGNER_WIDGET_EXPORT QGo4LineEdit : public QLineEdit {
   Q_OBJECT

   public:
      QGo4LineEdit(QWidget *parent=0, const char *name=0);
      virtual ~QGo4LineEdit();

   signals:
      void textDropped();

   protected:
      virtual void dropEvent(QDropEvent*);
      virtual void dragEnterEvent(QDragEnterEvent*);
};

#endif
