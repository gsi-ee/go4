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

#ifndef TGO4ANALYSISSTEPEXCEPTION_H
#define TGO4ANALYSISSTEPEXCEPTION_H

#include "TGo4RuntimeException.h"
#include "TString.h"

class TGo4AnalysisStep;

class TGo4AnalysisStepException : public TGo4RuntimeException {

  public:

     TGo4AnalysisStepException (TGo4AnalysisStep* step);

     virtual ~TGo4AnalysisStepException();

     Int_t Handle ();

     TGo4AnalysisStepException(const TGo4AnalysisStepException &right);

     TGo4AnalysisStepException & operator = (const TGo4AnalysisStepException & right);

     /**  Status of the last event. */
     Int_t GetProcessStatus() const { return fiProcessStatus; }

     /** Status message of last Process() call. */
     const char* GetStatusMessage() const { return fxStatusMessage.Data(); }

     /** Name of the throwing analysis step. */
     const char* GetStepName() { return fxStepName.Data(); }

  protected:
      /** @supplierCardinality 1 */
      TGo4AnalysisStep * fxStep;     //!

      TString fxStatusMessage;

      TString fxStepName;

      /** Contains current analysis step status value. To be checked by framework after Process().*/
      Int_t fiProcessStatus;

   private:
      TGo4AnalysisStepException();
};

#endif // #define TGO4TASKHANDLEREXCEPTION_H
