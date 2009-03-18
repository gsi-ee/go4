#ifndef QGO4TABLE_H
#define QGO4TABLE_H

#include <Q3Table>
#include <QtDesigner/QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT QGo4Table : public Q3Table {
   Q_OBJECT

   public:
    friend class QTableHeader;
    friend class QComboTableItem;
    friend class QCheckTableItem;
    friend class QTableItem;

     
     
      QGo4Table(QWidget *parent=0, const char *name=0);
      virtual ~QGo4Table();


   protected:
     virtual void sortColumn ( int col, bool ascending = TRUE, bool wholeRows = TRUE ); 
};

#endif
