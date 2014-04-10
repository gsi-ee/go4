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

#ifndef TGo4CreateException_h
#define TGo4CreateException_h

#include "TGo4ThreadException.h"
/**
 * Create a thread associated with the runnable.
 * @author J. Adamczewski
 * @author Go4 project-DVEE-GSI Darmstadt
 * @author Germany
 * @version 0.9
 * @since jul 2000-oct 2001*/
class TGo4CreateException : public TGo4ThreadException {

  public:
      TGo4CreateException(const TGo4CreateException &right);

      TGo4CreateException (TGo4Runnable* runnable, const char* threadname = 0);

      virtual ~TGo4CreateException();

      TGo4CreateException & operator=(const TGo4CreateException &right);

      /**
       * This is a default handler function for the respective exception type.
       */
      virtual Int_t Handle ();


  private:
      TGo4CreateException();

};


#endif



