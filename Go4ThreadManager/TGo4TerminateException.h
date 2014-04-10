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

#ifndef TGo4TerminateException_h
#define TGo4TerminateException_h

#include "TGo4ControlException.h"

class TGo4Runnable;

class TGo4ThreadManager;

/**
 * Exception which terminates the threadmanager and the application.
 * @author J. Adamczewski
 * @author Go4 project-DVEE-GSI Darmstadt
 * @author Germany
 * @version 0.9
 * @since jul 2000-oct 2001*/
class TGo4TerminateException : public TGo4ControlException {

  public:
      TGo4TerminateException(const TGo4TerminateException &right);

      TGo4TerminateException (TGo4ThreadManager* man);

      TGo4TerminateException (TGo4Runnable* runnable);

      virtual ~TGo4TerminateException();

      TGo4TerminateException & operator=(const TGo4TerminateException &right);

      virtual Int_t Handle ();



  private:
      TGo4TerminateException();

     TGo4ThreadManager *fxThreadManager; //!

};





#endif



