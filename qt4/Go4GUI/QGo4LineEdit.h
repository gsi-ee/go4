#ifndef QGO4LINEEDIT_H
#define QGO4LINEEDIT_H

#include <QtGui/QLineEdit>

#include <QDropEvent>
#include <QDragEnterEvent>

/**
 * @author Mohammad Al-Turany
 * @author S.Linev
 * @since 14.11.2001
 */

#ifdef WIN32

class QGo4LineEdit : public QLineEdit {

#else

#include <QtDesigner/QDesignerExportWidget>
class QDESIGNER_WIDGET_EXPORT QGo4LineEdit : public QLineEdit {

#endif
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
