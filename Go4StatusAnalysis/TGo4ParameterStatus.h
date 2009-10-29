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

#ifndef TGO4PARAMETERSTATUS_H
#define TGO4PARAMETERSTATUS_H

#include "TGo4ObjectStatus.h"

class TObjArray;
class TGo4Parameter;

/**
 * Status object for an analysis parameter. GUI will decide from
 * this the type of the remote object baseclass
 * @author J. Adamczewski
 * @since 26-FEB-2003
 */
class TGo4ParameterStatus : public TGo4ObjectStatus {

  public:

    TGo4ParameterStatus();

    TGo4ParameterStatus(TGo4Parameter* par, Bool_t membervalues = kFALSE);

    TGo4ParameterStatus(const char* parname, const char* parclass, TObjArray* items);

    virtual ~TGo4ParameterStatus();

    /** basic method to printout status information
      * on stdout; to be overridden by specific subclass */
    virtual Int_t PrintStatus(Text_t* buffer=0, Int_t buflen=0);

    TObjArray* GetMemberValues(Bool_t takeit = kFALSE);

    Bool_t UpdateParameterValues(TGo4Parameter* par);
    TGo4Parameter* CreateParameter();


  protected:
     TObjArray*  fxMemberValues;

  ClassDef(TGo4ParameterStatus,1)
};

#endif //TGO4PARAMETERSTATUS_H
