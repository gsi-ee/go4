// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TGO4COMMANDLINE_H
#define TGO4COMMANDLINE_H

#include "QGo4Widget.h"
#include "ui_TGo4CommandLine.h"

class TGo4CommandLine : public QGo4Widget, public Ui::TGo4CommandLine
{
   Q_OBJECT

   public:
      TGo4CommandLine(QWidget *parent = nullptr, const char* name = nullptr);

   public slots:

      virtual void FileSearchDialog();

      virtual void enterPressedSlot();

      virtual void ExecuteSlot();

      virtual void LoadHistory();

      virtual void PredefinedDialog();

      virtual void PrintHelp();

   protected:
      bool fbPythonBound{false};

};

#endif
