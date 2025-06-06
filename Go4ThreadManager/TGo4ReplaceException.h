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

#ifndef TGo4ReplaceException_h
#define TGo4ReplaceException_h

#include "TGo4ThreadException.h"

class TGo4Runnable;

/**
 * @brief Replace exception
 * @details
*   Exception with the effect of removing old TGo4Thread from
*   TGo4ThreadHandler and creating and starting new
*   TGo4Thread with new runnable fxNewRunnable.
*   If threadname is not specified in ctor, the thread of the
*   first runnable given (usually the calling runnable) is
*   replaced;
*   if new runnable is not given, the thread of the calling
*   runnable will be removed and a new thread with the
*   calling runnable will be created, added to the handler
*   list and started
* @ingroup go4_threadmgr
* @author J. Adamczewski
* @author Go4 project-DVEE-GSI Darmstadt
* @author Germany
* @version 0.9
* @since jul 2000-oct 2001*/

class TGo4ReplaceException : public TGo4ThreadException {
  public:
      TGo4ReplaceException() = delete;

      TGo4ReplaceException(const TGo4ReplaceException &right);

      TGo4ReplaceException(TGo4Runnable *runnable, TGo4Runnable *newrunnable = nullptr, const char *oldthreadname = nullptr);

      virtual ~TGo4ReplaceException();

      TGo4ReplaceException & operator=(const TGo4ReplaceException &right);

      Int_t Handle() override;

  protected:
      TGo4Runnable *fxNewRunnable{nullptr}; //!

};

#endif
