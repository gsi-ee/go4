#include "QGo4Table.h"
//#include <iostream>

QGo4Table::QGo4Table(QWidget* parent, const char* name) :
   Q3Table(parent, name)
{
}

QGo4Table::~QGo4Table()
{
}


void QGo4Table::sortColumn ( int col, bool ascending, bool wholeRows)
{
   //std::cout<<"QGo4Table::sortColumn("<<col<<","<<ascending <<", "<<wholeRows<<") "<<std::endl;
   Q3Table::sortColumn(col,ascending, true);
   
} 


