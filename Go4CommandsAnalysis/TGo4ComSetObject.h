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

#ifndef TGO4COMSETOBJECT_H
#define TGO4COMSETOBJECT_H

#include "TGo4AnalysisObjectCommand.h"

class TGo4AnalysisClient;
class TGo4Analysis;
class TGo4AnalysisObjectResult;
class TGo4RemoteCommand;
class TGo4Parameter;
class TGo4ParameterStatus;
class TGo4Condition;
class TGo4DynamicEntry;
class TGo4Picture;
class TH1;

/**
 * General setter command for any known object (parameter, condition, histogram, picture,..).
 * If an object of that name exists in the target folder, the new settings are applied on it.
 * If object does not exist in the full path, it is created there.
 * If the types do not match (case of parameters, conditions), an error message is
 * sent.
 * @author J. Adamczewski
 * @since 10-May-2005
 */
class TGo4ComSetObject : public TGo4AnalysisObjectCommand {
  public:

    TGo4ComSetObject();
    TGo4ComSetObject(const char* obname);
    virtual ~TGo4ComSetObject();
    Int_t ExeCom();
    /** Set internals of this command from specification of external
     * remote command (command container). Must be overloaded
     * for each specific command! */
    virtual void Set(TGo4RemoteCommand* remcon);

    void SetObject(TObject* ob) { fxObject=ob; }
  private:

    Int_t ExeSetParStatus(TGo4ParameterStatus* par);
    Int_t ExeSetPar(TGo4Parameter* par);
    Int_t ExeSetCon(TGo4Condition* con);
    Int_t ExeSetHis(TH1* his);
    Int_t ExeSetDyn(TGo4DynamicEntry* dyn);
    Int_t ExeSetPic(TGo4Picture* pic);
    Int_t ExeSetObj(TObject* ob);

    /**The object to be set. */
    TObject* fxObject;

    TGo4AnalysisClient* fxClient; //!
    TGo4Analysis* fxAna;//!


     /** structure to keep result of setting action */
    TGo4AnalysisObjectResult* fxResult; //!

  ClassDef(TGo4ComSetObject,1)
};

#endif //TGO4COMSETOBJECT_H
