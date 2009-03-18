#ifndef TGo4TestRunnable_h
#define TGo4TestRunnable_h

#include "TGo4Runnable.h"

class TPad;
class TCanvas;
class TH1D;
class TGo4TestThreadManager;

/**
 * Example of a user defined runnable subclass. Method Run() _must_ be implemented.
 * Methods ThreadCatch and UnexpectedCatch _can_ be overridden to change default
 * exception behaviour.
 * This runnable is able to do different actions depending on the fiMode parameter set
 * in the ctor (good for easy testing). However, the usual way is to create a dedicated
 * runnable class for a dedicated purpose and pass different
 * runnable classes to the different go4 threads (see Go4TaskHandler package for examples).
 ***********************************************************************************
 * Different actions in this runnable:
 * fiMode=0: Dummy
 *       printout a message every 5 seconds.
 * fiMode=1: Kill and create
 *       sleep 30s, then kill other threads- sleep 5s, then create other threads again
 * fiMode=2: Stop and start
 *      sleep 10s, then stop other threads- sleep 10s, then start other threads again
 * fiMode=3: Aborting all
 *       sleep 30s, then abort application by means of exception
 * fiMode=4: Histogramming
 *      creates histogram, fills it with random gaussian and draws it on pad
 *      depending on the runnable name ("Maker1", "Maker2"), different pads are used
 *      Maker 1 deletes histogram again each time, Maker 2 keeps initial histogram
 *      sleeps of 2s are used to slow down the action
 * fiMode=5: Recreator
 *       Cancel and recreate one thread by name ("First Thread") every 15 s.
 * fiMode=6: Exception tester
 *       sleep 60s, then throw different thread exceptions (latest: Restart exception)
 * fiMode=7: Histogram without drawing
 *       create histogram, fill it with random and delete it again
 *      for memory alloc tests independent from root gui
 * fiMode default: Undefined
 *      throw logic exception
 *********************************************************************************
 * NOTE1: Members of the runnable (histogram, canvas) are not deleted when the
 * TThread is killed. The runnable exists independent from the thread.
 * NOTE2: The user just has to create own runnables and a threadmanager subclasses,
 * subclassing of  TGo4Thread or other parts of the package are not required!
 * @author J. Adamczewski
 * @author Go4 project-DVEE-GSI Darmstadt
 * @author Germany
 * @author j.adamczewski@gsi.de
 * @version 0.9
 * @since jul 2000-oct 2001
 */
class TGo4TestRunnable : public TGo4Runnable  {

  public:
    //## Constructors (generated)
      TGo4TestRunnable();

      TGo4TestRunnable(const TGo4TestRunnable &right);

      TGo4TestRunnable (const char* name, TGo4ThreadManager* man, Int_t mode = 0);

      virtual ~TGo4TestRunnable();

  protected:

     /**
      *   The working function which runs in the
      *   thread
      */
      virtual Int_t Run (void* ptr);

       /**
         *   catch for exceptions occuring in workfunc
        **/
      virtual void ThreadCatch (TGo4Exception& ex);

       /**
         *          catch for all unexcpected exceptions happening in
         *         workfunc
        **/
      virtual void UnexpectedCatch ();


  private:

     /**
      *   mode number which can be used to create runnables
      *   with different actions in Workfunc; useful for testing
      *   without creating new subclasses of runnable...
      */
      Int_t fiMode;

    // Additional Private Declarations
      TPad* fxPad;
      TCanvas* fxCanvas;
      TH1D* fxHisto;
};



#endif



