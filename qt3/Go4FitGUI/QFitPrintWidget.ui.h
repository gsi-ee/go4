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

/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include <sstream>
using namespace std;

void QFitPrintWidget::SetDrawOption( const QString & option )
{
   fxDrawOption = option;
}

void QFitPrintWidget::FillSpecificData() {
  if (GetObject()) {

      cout.flush();

      ostringstream strout;

      streambuf* cout_buffer = cout.rdbuf();

      cout.rdbuf(strout.rdbuf());

      GetObject()->Print(fxDrawOption);

      cout << endl;

      cout.flush();

      cout.rdbuf(cout_buffer);

      PrintText->setText(strout.str().c_str());
  }
}
