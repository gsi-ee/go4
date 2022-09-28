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

#ifndef TGO4DISPLAY_H
#define TGO4DISPLAY_H

#include "TGo4Master.h"

class TGo4Status;
class TGo4DisplayLoggerTimer;
class TGo4DisplayDrawerTimer;
class TGo4AnalysisProxy;

class TGo4Display : public TGo4Master {
  public:

    TGo4Display(Bool_t isserver=kTRUE);
    virtual ~TGo4Display();

    void DisplayLog(TGo4Status * Status);
    void DisplayData(TObject*);

    void SetAnalysis(TGo4AnalysisProxy* an) { fxAnalysis = an; }

 /** Add functionality to reset gui appearance here:*/
    Bool_t DisconnectSlave(const char *name = nullptr,
                           Bool_t waitforslave = kTRUE) override;

  private:

    /** Timer for drawing job */
    TGo4DisplayDrawerTimer* fxDrawTimer{nullptr};         //!

    /** Timer for status logging job */
    TGo4DisplayLoggerTimer* fxLogTimer{nullptr};          //!

    TGo4AnalysisProxy*      fxAnalysis{nullptr};          //!
};

#endif //TGO4DISPLAY_H
