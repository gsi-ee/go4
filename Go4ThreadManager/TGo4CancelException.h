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

#ifndef TGo4CancelException_h
#define TGo4CancelException_h

#include "TGo4ThreadException.h"

/**
 * @brief Cancel thread associated with runnable or with threadname.
 * @ingroup go4_threadmgr
 * @author J. Adamczewski
 * @author Go4 project-DVEE-GSI Darmstadt
 * @author Germany
 * @version 0.9
 * @since jul 2000-oct 2001*/
class TGo4CancelException : public TGo4ThreadException {
public:

   TGo4CancelException() = delete;

   TGo4CancelException(const TGo4CancelException &right);

   TGo4CancelException(TGo4Runnable *runnable, const char *threadname = nullptr);

   virtual ~TGo4CancelException();

   TGo4CancelException &operator=(const TGo4CancelException &right);

   /**
    * This is a default handler function for the respective exception type.
    */
   Int_t Handle() override;

};

#endif
