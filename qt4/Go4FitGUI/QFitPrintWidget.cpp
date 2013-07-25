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

#include "QFitPrintWidget.h"

#include "Riostream.h"
#include "Rstrstream.h"
#include "TObject.h"

#include <sstream>
using namespace std;



QFitPrintWidget::QFitPrintWidget(QWidget *parent, const char* name)
         : QFitWidget(parent, name)
{
   setupUi(this);
}


void QFitPrintWidget::SetDrawOption( const QString & option )
{
   fxDrawOption = option;
}

void QFitPrintWidget::FillSpecificData()
{
   if (GetObject()) {

      std::cout.flush();

      std::ostringstream strout;

      std::streambuf* ccc_buffer = std::cout.rdbuf();

      std::cout.rdbuf(strout.rdbuf());

      GetObject()->Print(fxDrawOption.toAscii().constData());

      std::cout << std::endl;

      std::cout.flush();

      std::cout.rdbuf(ccc_buffer);

      PrintText->setText(strout.str().c_str());
   }
}
