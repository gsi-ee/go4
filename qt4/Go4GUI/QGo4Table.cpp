#include "QGo4Table.h"

QGo4Table::QGo4Table(QWidget* parent, const char* name) :
   Q3Table(parent, name)
{
}

QGo4Table::~QGo4Table()
{
}


void QGo4Table::sortColumn ( int col, bool ascending, bool wholeRows)
{
   Q3Table::sortColumn(col,ascending, true);
}


