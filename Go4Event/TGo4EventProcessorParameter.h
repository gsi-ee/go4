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

#ifndef TGO4EVENTPROCESSORPARAMETER_H
#define TGO4EVENTPROCESSORPARAMETER_H

#include "TGo4Parameter.h"

/**
 * Basic type for all classes containing information
 * to parametrize the event processor. For use in the
 * event factory; factory method gets (possibly subclassed)
 * parameter object which is suited to the concrete user
 * factory. Name string can be used as filename, e.g.
 */
class TGo4EventProcessorParameter : public TGo4Parameter {
   public:
      TGo4EventProcessorParameter() ;

      TGo4EventProcessorParameter(const char* name, Int_t id = 0);

      virtual ~TGo4EventProcessorParameter() ;

      Int_t GetID() const { return fiID; }

      /** basic method to printout status information
        * on stdout; to be overridden by specific subclass */
      virtual Int_t PrintParameter(Text_t* buffer=0, Int_t buflen=0);

      /** update contents of paramter class with external object. */
      virtual Bool_t UpdateFrom(TGo4Parameter* rhs);

   private:

      /** Identity number to specify the kind. Optional, default is 0. */
      Int_t fiID;

  ClassDef(TGo4EventProcessorParameter,1)
};

#endif //TGO4EVENTPROCESSORPARAMETER_H
