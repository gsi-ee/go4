#include "QGo4Table.h"
#include <iostream>

QGo4Table::QGo4Table(QWidget* parent, const char* name) :
   QTable(parent, name)
{
}

QGo4Table::~QGo4Table()
{
}


void QGo4Table::sortColumn ( int col, bool ascending, bool wholeRows)
{
   //std::cout<<"QGo4Table::sortColumn("<<col<<","<<ascending <<", "<<wholeRows<<") "<<std::endl;
   QTable::sortColumn(col,ascending, true);
   
} 


