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

#ifndef TQROOTDIALOG_H
#define TQROOTDIALOG_H

#include <Qt3Support/q3vbox.h>
#include <QtCore/QList>
#include <QtGui/QCloseEvent>

class TObject;
class TMethod;
class TCanvas;
class QLineEdit;
class TQCanvasMenu;

/**
* TQRootDialog
*
* A TQRootDialog is used to prompt for the arguments of an object's
* member function. It is called by the TQCanvasMenu class
*
* @see TQCanvasMenu, QList, QVBox, TMethod, TCanvas
*
*
* @authors Denis Bertini <d.bertini@gsi.de>
* @version 2.0
*/


class TQRootDialog: public Q3VBox {
   Q_OBJECT

   public:
      TQRootDialog(QWidget *parent, const char *name, Qt::WFlags f=0,
                   TObject *obj=0,TMethod *meth=0);

      virtual ~TQRootDialog();

      void add(const char* argname, const char* value, const char* type);
      void popup();
      void setTCanvas(TCanvas* aCanvas) { fCurCanvas=aCanvas; }
      void SetCanMenu(TQCanvasMenu* menu) { fMenu = menu; }

   public slots:
       void receive(); // to be replaced
       void executeMethod();

   protected:
       void closeEvent( QCloseEvent* ce);

   protected:
       Q3VBox*           fArgBox;
       QList<QLineEdit*> aList;
       TObject*         fCurObj;
       TObject*         fPrevObj;
       TMethod*         fCurMethod;
       TMethod*         fPrevMethod;
       TCanvas*         fCurCanvas;
       QWidget*         fParent;
       TQCanvasMenu*    fMenu;
};

#endif
