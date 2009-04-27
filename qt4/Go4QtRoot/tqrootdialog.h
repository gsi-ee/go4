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

#include <QtCore/QVector>
#include <QtGui/QDialog>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLineEdit>

/**
* TQRootDialog
*
* A TQRootDialog is used to prompt for the arguments of an object's
* member function. It is called by the TQRootCanvas class
*
* @see TQRootCanvas
*
*
* @authors Denis Bertini <d.bertini@gsi.de>, Sergey Linev
* @version 2.0
*/


class TQRootDialog: public QDialog {
   Q_OBJECT

   public:
      TQRootDialog();

      void addArg(const char* argname, const char* value, const char* type);

      QString getArg(int n);

   protected:
      QVBoxLayout *argLayout;

      QVector<QLineEdit*> fArgs;
};

#endif
