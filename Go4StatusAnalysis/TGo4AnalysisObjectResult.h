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

#ifndef TGO4ANALYSISOBJECTRESULT_H
#define TGO4ANALYSISOBJECTRESULT_H

#include "TGo4Status.h"

#include "TString.h"
#include "TGo4AnalysisObjectNames.h"


/**
 * This status is sent back to GUI as a result structure after a command
 * has been executed in analysis to modify or create an object.
 * GUI will receive most recent names list, full path to last modified
 * object and optionally some command token to execute (edit, plot, error)
 * @since 5/2005
 * @author J. Adamczewski
 */


 /** Go4 result action values*/
    enum Go4ResultAction_t
    {
       kGo4ActionNul,       // NOP
       kGo4ActionRefresh,   // Refresh this object
       kGo4ActionPlot,      // Display object
       kGo4ActionEdit,      // Get work object into editor
       kGo4ActionError      // Display Error message
    };


class TGo4AnalysisObjectResult : public TGo4Status {


  public:

   TGo4AnalysisObjectResult();

   TGo4AnalysisObjectResult(const char* name);

   virtual ~TGo4AnalysisObjectResult();

   void SetNamesList(TGo4AnalysisObjectNames* n) { fxNamesList = n;}

    /** Take out the nameslist. */
    TGo4AnalysisObjectNames * GetNamesList(Bool_t chown=kTRUE);

    const char* GetObjectFullName() const { return fxFullName.Data(); }

    void SetObjectFullName(const char* nam){fxFullName=nam;}

    const char* GetMessage() const { return fxMessage.Data(); }

    void SetMessage(const char* nam){fxMessage=nam;}

    void SetAction(Go4ResultAction_t com){fiAction=com;}

    Go4ResultAction_t Action() const { return fiAction; }

    /** basic method to printout status information
     * on stdout; to be overridden by specific subclass */
    virtual Int_t PrintStatus(Text_t* buffer=0, Int_t buflen=0);

  private:

    /** Last state of the objects in analysis after modification*/
    TGo4AnalysisObjectNames * fxNamesList;
    /** Full name and path of the object last modified*/
    TString fxFullName;
    /** This contains an action token to execute on gui side*/
    Go4ResultAction_t fiAction;
     /** Optional error or info message*/
    TString fxMessage;

  ClassDef(TGo4AnalysisObjectResult,1)
};

#endif //TGO4ANALYSISOBJECTRESULT_H
