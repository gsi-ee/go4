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

#ifndef TGo4TestThreadManager_h
#define TGo4TestThreadManager_h

#include "TGo4ThreadManager.h"

class TPad;
class TCanvas;
class TGo4TestRunnable;

/**
 * Example of a user subclass of the threadmanager. This test threadmanager
 * creates the runnables in its constructor and adds them to the threadhandler.
 * NOTE: The Launch() method must be called at the end of the user threadmanager
 * constructor to let all things begin.
 * @author J. Adamczewski(at)gsi.de
 * @author Go4 project-DVEE-GSI Darmstadt
 * @author Germany
 * @version 0.9
 * @since jul 2000-oct 2001
 */
class TGo4TestThreadManager : public TGo4ThreadManager
{

  public:
      TGo4TestThreadManager (const char *name);

      virtual ~TGo4TestThreadManager();

      /**
       *   general testfunction which can be called from any
       *   thread runnable
       */
      virtual Int_t TestAction();

      TCanvas *GetCanvas() const { return fxCanvas; }

      TPad *GetPad1() const { return fxPad1; }

      TPad *GetPad2() const { return fxPad1; }


  protected:
    // Data Members for Associations

      TGo4TestRunnable *fxControlRunnable{nullptr}; //!

  private:

      TCanvas *fxCanvas{nullptr};   //!
      TPad *fxPad1{nullptr};        //!
      TPad *fxPad2{nullptr};        //!

   ClassDefOverride(TGo4TestThreadManager,1)
};

#endif
