#include "TGo4SocketSignalHandler.h"

#include "Riostream.h"


Int_t TGo4SocketSignalHandler::fgiLastSignal = 0; // unix signal id

TGo4SocketSignalHandler::TGo4SocketSignalHandler(Int_t signum, Bool_t enabled)
{
   SetSignalAction(signum,enabled);
   //cout << "set member function as signal handler"<< endl;
}

TGo4SocketSignalHandler::~TGo4SocketSignalHandler()
{
}

void TGo4SocketSignalHandler::Handle(int signum)
{
   SetSignalAction(signum,0);
   //cout << "got a signal " << signum << " ("<< getpid() << ") Id: ";
// optional part:
//   if(signum == SIGWINCH)
//      {
//         //cout << "got a SIGWINCH\n";
//         //TGo4SocketSignalHandler::fgiLastSignal = SIGWINCH;
//
//       }
// end optional part
   TGo4SocketSignalHandler::fgiLastSignal = signum;
   SetSignalAction(signum,1);
}

#ifndef WIN32

void TGo4SocketSignalHandler::SetSignalAction(Int_t signum, Bool_t enabled)
{
  struct sigaction new_action, old_action;

  /* Set up the structure to specify the new action. */
  if(enabled)
    new_action.sa_handler = TGo4SocketSignalHandler::Handle;
  else
    new_action.sa_handler = SIG_IGN;

  sigemptyset (&new_action.sa_mask);
  new_action.sa_flags = 0;
  sigaction (signum, NULL, &old_action);
  if(enabled)
    sigaction (signum, &new_action, NULL);
}

#else

// do nothing in windows

void TGo4SocketSignalHandler::SetSignalAction(Int_t signum, Bool_t enabled)
{
}

#endif

Int_t TGo4SocketSignalHandler::GetLastSignal()
{
   return fgiLastSignal;
}

void TGo4SocketSignalHandler::SetLastSignal(Int_t v)
{
   fgiLastSignal = v;
}


