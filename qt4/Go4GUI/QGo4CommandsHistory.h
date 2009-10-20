#ifndef QGO4COMMANDSHISTORY_H
#define QGO4COMMANDSHISTORY_H

#include <QtGui/QComboBox>

#include <QtCore/QStringList>

#ifdef WIN32
class QGo4CommandsHistory : public QComboBox {

#else

#include <QtDesigner/QDesignerExportWidget>
class QDESIGNER_WIDGET_EXPORT QGo4CommandsHistory : public QComboBox {

#endif

   Q_OBJECT

   public:
      QGo4CommandsHistory(QWidget* w, const char* name = 0);
      virtual ~QGo4CommandsHistory();

      QStringList getHistory(int max = 50);

Q_SIGNALS:
      void enterPressedSingal();

   protected:
      virtual void keyPressEvent(QKeyEvent* e);
};

#endif
