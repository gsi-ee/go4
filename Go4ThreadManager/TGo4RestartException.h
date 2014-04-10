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

#ifndef TGo4RestartException_h
#define TGo4RestartException_h

#include "TGo4ThreadException.h"

/**
 * Exception which cancels and recreates a thread associated with runnable,
 * or associated with a given name. Similar to TGo4ReplaceException, but uses
 * Recreate method of taskhandler.
 * @author J. Adamczewski
 * @author Go4 project-DVEE-GSI Darmstadt
 * @author Germany
 * @version 0.9
 * @since jul 2000-oct 2001*/
class TGo4RestartException : public TGo4ThreadException  {
  public:
      TGo4RestartException(const TGo4RestartException &right);

      /**
       * Restart thread associated with runnable or with threadname.
       * Old thread will be canceled and created again.
       */
      TGo4RestartException (TGo4Runnable* runnable, const char* threadname = 0);

      virtual ~TGo4RestartException();

      TGo4RestartException & operator=(const TGo4RestartException &right);

      virtual Int_t Handle ();

  private:
      TGo4RestartException();
};

#endif
