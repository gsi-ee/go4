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

#ifndef TGO4MBSFILEPARAMETER_H
#define TGO4MBSFILEPARAMETER_H

#include "TGo4MbsSourceParameter.h"

#include "TString.h"

class TGo4MbsFileParameter : public TGo4MbsSourceParameter {
  public:
    TGo4MbsFileParameter() ;

    TGo4MbsFileParameter(const char* name);

    virtual ~TGo4MbsFileParameter();

    /** Name of the Tagfile */
    const char* GetTagName() const { return fxTagFile.Data(); }

    /** Name of the Tagfile */
    void SetTagName(const char* name) { fxTagFile = name; }

    /** basic method to printout status information
      * on stdout; to be overridden by specific subclass */
    virtual Int_t PrintParameter(Text_t* buffer=0, Int_t buflen=0);

    /** update contents of paramter class with external object. */
    virtual Bool_t UpdateFrom(TGo4Parameter* rhs);

  private:

    /** Name of the tagfile associated with the listmode event file. */
    TString fxTagFile;

  ClassDef(TGo4MbsFileParameter, 2)

};

#endif //TGO4MBSFILEPARAMETER_H
