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

#ifndef TGo4StartException_h
#define TGo4StartException_h

#include "TGo4ThreadException.h"

/**
 * @brief start thread associated with runnable.
 * @ingroup go4_threadmgr
 * @author J. Adamczewski
 * @author Go4 project-DVEE-GSI Darmstadt
 * @author Germany
 * @version 0.9
 * @since jul 2000-oct 2001*/
class TGo4StartException : public TGo4ThreadException {

  public:
      TGo4StartException() = delete;

      TGo4StartException(const TGo4StartException &right);

      TGo4StartException (TGo4Runnable *runnable, const char *threadname = nullptr);

      virtual ~TGo4StartException();

      TGo4StartException & operator=(const TGo4StartException &right);

      Int_t Handle() override;

};

#endif
