// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

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

#ifndef QROOTDIALOG_H
#define QROOTDIALOG_H

#include <QtCore/QVector>
#include <QDialog>

class QLineEdit;
class QVBoxLayout;

/**
* QRootDialog
*
* A QRootDialog is used to prompt for the arguments of an object's
* member function. It is called by the QRootCanvas class
*
* @see QRootCanvas
*
*
* @authors Denis Bertini <d.bertini@gsi.de>, Sergey Linev
* @version 2.0
*/


class QRootDialog: public QDialog {
   Q_OBJECT

   public:
      QRootDialog();

      void addArg(const char* argname, const char* value, const char* type);

      QString getArg(int n);

   protected:
      QVBoxLayout *argLayout;

      QVector<QLineEdit*> fArgs;
};

#endif
