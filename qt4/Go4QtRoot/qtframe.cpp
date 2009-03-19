//Author : Denis Bertini 01.11.2000

/**************************************************************************
* Copyright (C) 2000- Gesellschaft f. Schwerionenforschung, GSI           *
*                     Planckstr. 1, 64291 Darmstadt, Germany              *
*                     All rights reserved.                                *
* Contact:            http://go4.gsi.de                                   *
*                                                                         *
* This software can be used under the license agreements as stated in     *
* Go4License.txt file which is part of the distribution.                  *
***************************************************************************/

#include "qtframe.h"

#include <QtGui/qmessagebox.h>
#include <QtGui/QCloseEvent>
#include "tqrootcanvas.h"

using namespace std;

QTFrame::QTFrame(QWidget * parent, const char * name , Qt::WFlags f )
    : Q3MainWindow( parent, name, f)
{
  // printf("ctor QTFrame \n");
  aCanvas=0;

}

QTFrame::~QTFrame()
{
}

void QTFrame::setCanvas(TQRootCanvas* a)
{
   aCanvas = a;
   setCentralWidget( aCanvas );
}

void QTFrame::closeEvent( QCloseEvent* ce )
{
  // up to now just accept the close event
  ce->accept();
}

