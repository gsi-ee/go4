#ifndef QFITWIDGET_H
#define QFITWIDGET_H

#include <QtGui/QWidget>

class TObject;
class QFitItem;
class TGo4Fitter;
class TGo4FitPanel;

#ifdef WIN32
class QFitWidget : public QWidget {
#else
#include <QtDesigner/QDesignerExportWidget>
class QDESIGNER_WIDGET_EXPORT QFitWidget : public QWidget {
#endif


   Q_OBJECT

   public:
      QFitWidget( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
      ~QFitWidget();

      virtual QFitItem* GetItem();
      virtual TObject* GetObject();
      virtual TGo4Fitter* GetFitter();
      virtual void SetItem(TGo4FitPanel* panel, QFitItem* iItem);
      virtual void FillWidget();
      virtual void FillSpecificData();
      virtual void UpdateWidgetItem(bool trace);
      virtual void SetWidgetItemText(bool trace);
      virtual void UpdateItemsOfType(int typ, bool allitems);

   protected:
      TGo4FitPanel* fxPanel;
      QFitItem* fxItem;
      bool fbFillWidget;

      virtual void closeEvent(QCloseEvent* ce);
      virtual void contextMenuEvent(QContextMenuEvent* e);
};

#endif

