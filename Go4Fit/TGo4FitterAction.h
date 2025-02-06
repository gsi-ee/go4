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

#ifndef TGO4FITTERACTION_H
#define TGO4FITTERACTION_H

#include "TGo4FitNamed.h"

class TGo4FitterAbstract;

/**
 * Basic class for objects, which performs actions on fitter.
 * @ingroup go4_fit
 */
class TGo4FitterAction : public TGo4FitNamed {
   public:

      /**
       * Default constructor.
       */
      TGo4FitterAction();

      /**
       * Creates TGo4FitterAction object with given name and title.
       */
      TGo4FitterAction(const char *Name, const char *Title, TNamed *Owner = nullptr);
      virtual ~TGo4FitterAction();

      virtual void DoAction(TGo4FitterAbstract *) = 0;

      virtual Bool_t CanChangeFitter() const { return kFALSE; }
      virtual Bool_t NeedBuffers() const { return kFALSE; }

   ClassDefOverride(TGo4FitterAction,1)
};

#endif // TGO4FITTERACTION_H
