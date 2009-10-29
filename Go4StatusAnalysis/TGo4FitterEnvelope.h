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

#ifndef TGO4FITTERENVELOPE_H
#define TGO4FITTERENVELOPE_H

#include "TGo4Parameter.h"

#include "TGo4Fitter.h"

/**
* This class is a container for one fitter to
* be send between gui and analysis and back. It is treated in the scope
* of the go4 parameter mechanism.
* @author J. Adamczewski
* @since 8-apr-2003
*/

class TGo4FitterEnvelope : public TGo4Parameter {

  public:
    TGo4FitterEnvelope();
    TGo4FitterEnvelope(const char* name, TGo4Fitter* fitter=0);
    virtual ~TGo4FitterEnvelope();
    Int_t PrintParameter(Text_t * n, Int_t);
    Bool_t UpdateFrom(TGo4Parameter *);

    virtual void Clear(Option_t* opt="");

    /** Access to the contained fitter object.
      * If change owner is kTRUE, this fitter envelope will lose
      * its fitter to the invoking client and is empty after
      * this call. */
    TGo4Fitter* GetFitter(Bool_t chown=kFALSE);

     /** Set new fitter into envelope. Previous fitter is deleted. */
     void SetFitter(TGo4Fitter* fitter);

  private:
    TGo4Fitter* fxFitter;

  ClassDef(TGo4FitterEnvelope,1)
};

#endif //TGO4FITTERENVELOPE_H
