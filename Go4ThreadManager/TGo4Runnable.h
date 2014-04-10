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

#ifndef TGo4Runnable_H
#define TGo4Runnable_H

#include "TNamed.h"

#include "TGo4Exception.h"

class TGo4Thread;
class TGo4ThreadManager;

/**
*   Base class for all go4 runnables. Virtual method Run() is working in
* the thread, this must be overridden by the user runnables.
* @author J. Adamczewski
* @author Go4 project-DVEE-GSI Darmstadt
* @author Germany
* @version 0.9
* @since jul 2000-oct 2001*/

class TGo4Runnable : public TNamed {
  friend class TGo4Thread;

  public:
      TGo4Runnable(const TGo4Runnable &right);

      TGo4Runnable (const char* name, TGo4ThreadManager* man);

      virtual ~TGo4Runnable();

      void SetThread (TGo4Thread* gthread) { fxGo4Thread=gthread; }

      TGo4Thread* GetThread() const { return fxGo4Thread; }

      TGo4ThreadManager* GetThreadManager() const { return fxManager; }

   protected:

      TGo4Runnable();

      /** The working function which runs in the thread */
      virtual Int_t Run (void* ptr) = 0;

      /** Function which is called once before the Run() method on threadstart */
      virtual Int_t PreRun (void* ptr);

      /** Function which is called once after the Run()method on threadstop */
      virtual Int_t PostRun (void* ptr);

      /** catch for exceptions occuring in workfunc */
      virtual void ThreadCatch(TGo4Exception& ex);

      /** catch for all unexcpected exceptions happening in workfunc */
      virtual void UnexpectedCatch ();

      /** The threadmanager which is responsible for our thread.
        * @supplierCardinality 1
        * @clientCardinality 0..*  */
      TGo4ThreadManager *fxManager; //!

      /** Backlink to thread which runs this.
        * @supplierCardinality 1
        * @clientCardinality 1 */
      TGo4Thread *fxGo4Thread;   //!
};

#endif
